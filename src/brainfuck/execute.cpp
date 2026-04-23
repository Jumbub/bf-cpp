#include "execute.h"

#include <sys/mman.h>

#include <cstdint>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace brainfuck {

// ---------------------------------------------------------------------------
// I/O helpers. Called from JITted code via absolute-address `call rax`.
// ---------------------------------------------------------------------------

void output(const int8_t output, const Value times) {
  for (int i = 0; i < times; i++) {
    std::cout << static_cast<char>(output);
  }
}

void input(int8_t* character, const Value times) {
  for (int i = 0; i < times; i++) {
    char input;
    std::cin >> std::noskipws >> input;
    if (!std::cin.eof()) {
      *character = input;
    }
  }
}

extern "C" void bf_output_c(int8_t c, int64_t times) { output(c, times); }
extern "C" void bf_input_c(int8_t* p, int64_t times) { input(p, times); }

// ---------------------------------------------------------------------------
// JIT: compile the parsed Instruction[] into native x86-64 machine code and
// execute it.
//
// Calling convention of the generated function:
//   void jit_entry(int8_t* data);
//   rdi = data pointer on entry.
//   Inside the body: rbx holds the data pointer (callee-saved, survives calls
//   to bf_output_c / bf_input_c).
//
// Emission is two-pass:
//   Pass 1 -- walk instructions, compute the byte offset of each instruction
//             given its type/value/move. Every op size is deterministic from
//             the parsed Instruction, so no back-patching is needed.
//   Pass 2 -- walk again, emit exactly the bytes promised in pass 1. Branch
//             targets (`[`, `]`, and the DATA_TRANSFER end-of-body fallthrough)
//             are resolved from the offset table.
//
// x86-64 System V ABI, Linux-only.
// ---------------------------------------------------------------------------

namespace {

// 6 bytes: add rsp,8 ; pop rbx ; ret
constexpr size_t kEpilogueSize = 6;

// Pre-move applied before each instruction's body: `add rbx, imm32` = 7 bytes.
// Emitted only when move != 0.
constexpr size_t kPreMoveSize = 7;

// Per-op body sizes (excluding the optional pre-move prefix).
// These match the encoder below exactly.
size_t bodySize(const Instruction& ins) {
  switch (ins.type) {
    case NOOP:
      return 0;
    case DONE:
      return kEpilogueSize;
    case DATA_ADD:
      return 3;  // 80 03 ib
    case DATA_SET_FROM_INPUT:
      return 3 + 5 + 10 + 2;  // mov rdi,rbx ; mov esi,imm32 ; mov rax,imm64 ; call rax
    case DATA_PRINT:
      return 3 + 5 + 10 + 2;  // movsx edi,[rbx] ; mov esi,imm32 ; mov rax,imm64 ; call rax
    case INSTRUCTION_POINTER_SET_IF_ZERO:
      return 3 + 6;  // cmp byte [rbx],0 ; je rel32
    case INSTRUCTION_POINTER_SET_IF_NOT_ZERO:
      // Scan-vs-normal is decided at emit time using the offset table; both
      // encodings below must agree here. The scan form is 14 bytes, the
      // normal form is 9 bytes. We can't tell which one this `]` is from the
      // instruction alone -- we need to know the target offset. Resolved via
      // isScanLoop() below, which only needs the parsed value + its own index.
      return 0;  // handled out-of-line
    case DATA_TRANSFER: {
      // movzx r8d,[rbx] (4) + N * (imul eax,r8d,imm32 (7) + add [rbx+disp32],al (6)) + mov byte [rbx],0 (3)
      const size_t n = static_cast<size_t>(ins.value);
      return 4 + n * (7 + 6) + 3;
    }
    case DATA_TRANSFER_META:
      return 0;  // absorbed by the preceding DATA_TRANSFER
    case DATA_POINTER_ADD:
      throw std::runtime_error("DATA_POINTER_ADD should have been squashed by parse.cpp");
  }
  throw std::runtime_error("bodySize: unknown instruction type");
}

// `]` is a scan loop iff it jumps to itself -- parser sets value = idx + 1
// for self-loops (e.g. `[>]` becomes `[` followed by `]` with move != 0 at
// the next index, and value points back to index+1 == self).
bool isScanLoop(size_t idx, const Instruction& ins) {
  return ins.type == INSTRUCTION_POINTER_SET_IF_NOT_ZERO &&
         static_cast<size_t>(ins.value) == idx;
}

// Size of a `]` body given whether it's a scan loop.
size_t closeBracketSize(bool scan) {
  // scan:   cmp [rbx],0 (3) + je rel8 (2) + add rbx,imm32 (7) + jmp rel8 (2)
  // normal: cmp [rbx],0 (3) + jne rel32 (6)
  return scan ? 14 : 9;
}

class Buf {
 public:
  explicit Buf(size_t reserve) { bytes_.reserve(reserve); }

  void u8(uint8_t v) { bytes_.push_back(v); }
  void u32(uint32_t v) {
    for (int i = 0; i < 4; i++) bytes_.push_back(static_cast<uint8_t>(v >> (i * 8)));
  }
  void u64(uint64_t v) {
    for (int i = 0; i < 8; i++) bytes_.push_back(static_cast<uint8_t>(v >> (i * 8)));
  }

  size_t pos() const { return bytes_.size(); }
  const std::vector<uint8_t>& data() const { return bytes_; }

 private:
  std::vector<uint8_t> bytes_;
};

// Helpers that assert the emitted byte count matches the size bodySize()
// promised. Any mismatch means the offset table is lying and the program will
// jump into the middle of an instruction -- very loud failure -> catch early.
#define EMIT_BEGIN(expected) const size_t _emit_start = buf.pos(); const size_t _emit_expected = (expected)
#define EMIT_END() do { if (buf.pos() - _emit_start != _emit_expected) throw std::runtime_error("JIT size mismatch"); } while (0)

// `add rbx, imm32`  -> 48 81 C3 id
void emitPreMove(Buf& buf, int64_t move) {
  buf.u8(0x48); buf.u8(0x81); buf.u8(0xC3);
  buf.u32(static_cast<uint32_t>(static_cast<int32_t>(move)));
}

// `add byte ptr [rbx], imm8`  -> 80 03 ib
void emitDataAdd(Buf& buf, int64_t value) {
  EMIT_BEGIN(3);
  buf.u8(0x80); buf.u8(0x03); buf.u8(static_cast<uint8_t>(value & 0xFF));
  EMIT_END();
}

// call through r/m64, target loaded into rax as a 64-bit immediate so it works
// no matter how far apart the JIT buffer and libbrainfuck-execute end up.
//   mov rax, imm64   -> 48 B8 iq  (10 bytes)
//   call rax         -> FF D0     (2 bytes)
void emitMovRaxImm64Call(Buf& buf, void* fn) {
  buf.u8(0x48); buf.u8(0xB8);
  buf.u64(reinterpret_cast<uint64_t>(fn));
  buf.u8(0xFF); buf.u8(0xD0);
}

// DATA_PRINT: output(*rbx, value);
//   movsx edi, byte ptr [rbx]  -> 0F BE 3B      (3)
//   mov   esi, imm32           -> BE id         (5)
//   mov   rax, &bf_output_c    -> 48 B8 iq      (10)
//   call  rax                  -> FF D0         (2)
void emitDataPrint(Buf& buf, int64_t value) {
  EMIT_BEGIN(20);
  buf.u8(0x0F); buf.u8(0xBE); buf.u8(0x3B);
  buf.u8(0xBE); buf.u32(static_cast<uint32_t>(static_cast<int32_t>(value)));
  emitMovRaxImm64Call(buf, reinterpret_cast<void*>(&bf_output_c));
  EMIT_END();
}

// DATA_SET_FROM_INPUT: input(rbx, value);
//   mov  rdi, rbx              -> 48 89 DF      (3)
//   mov  esi, imm32            -> BE id         (5)
//   mov  rax, &bf_input_c      -> 48 B8 iq      (10)
//   call rax                   -> FF D0         (2)
void emitDataInput(Buf& buf, int64_t value) {
  EMIT_BEGIN(20);
  buf.u8(0x48); buf.u8(0x89); buf.u8(0xDF);
  buf.u8(0xBE); buf.u32(static_cast<uint32_t>(static_cast<int32_t>(value)));
  emitMovRaxImm64Call(buf, reinterpret_cast<void*>(&bf_input_c));
  EMIT_END();
}

// `[`: if *data == 0 jump to instruction after matching `]`.
//   cmp byte ptr [rbx], 0      -> 80 3B 00      (3)
//   je  rel32 -> target        -> 0F 84 id      (6)
void emitOpen(Buf& buf, int32_t rel32) {
  EMIT_BEGIN(9);
  buf.u8(0x80); buf.u8(0x3B); buf.u8(0x00);
  buf.u8(0x0F); buf.u8(0x84); buf.u32(static_cast<uint32_t>(rel32));
  EMIT_END();
}

// `]` normal: if *data != 0 jump to instruction after matching `[`.
//   cmp byte ptr [rbx], 0      -> 80 3B 00      (3)
//   jne rel32                  -> 0F 85 id      (6)
void emitCloseNormal(Buf& buf, int32_t rel32) {
  EMIT_BEGIN(9);
  buf.u8(0x80); buf.u8(0x3B); buf.u8(0x00);
  buf.u8(0x0F); buf.u8(0x85); buf.u32(static_cast<uint32_t>(rel32));
  EMIT_END();
}

// `]` scan loop (empty body with nonzero move): while (*data) rbx += move;
//   L1: cmp byte ptr [rbx], 0  -> 80 3B 00      (3)
//       je  L2 (rel8 +9)       -> 74 09         (2)
//       add rbx, imm32         -> 48 81 C3 id   (7)
//       jmp L1 (rel8 -14)      -> EB F2         (2)
//   L2:
void emitCloseScan(Buf& buf, int64_t move) {
  EMIT_BEGIN(14);
  buf.u8(0x80); buf.u8(0x3B); buf.u8(0x00);   // L1: cmp [rbx],0
  buf.u8(0x74); buf.u8(0x09);                 //     je L2   (skip the 9 bytes below)
  buf.u8(0x48); buf.u8(0x81); buf.u8(0xC3);   //     add rbx, imm32
  buf.u32(static_cast<uint32_t>(static_cast<int32_t>(move)));
  buf.u8(0xEB); buf.u8(0xF2);                 //     jmp L1  (back 14 bytes)
  EMIT_END();
}

// DATA_TRANSFER + N METAs unrolled:
//   movzx r8d, byte ptr [rbx]      -> 44 0F B6 03   (4)
//   for each meta (value V, move M):
//     imul eax, r8d, imm32(V)      -> 41 69 C0 id   (7)
//     add  byte ptr [rbx+disp32(M)], al -> 00 83 id (6)
//   mov  byte ptr [rbx], 0         -> C6 03 00      (3)
void emitDataTransfer(
    Buf& buf,
    size_t n,
    const Instruction* metas  // pointer to the first META
) {
  EMIT_BEGIN(4 + n * (7 + 6) + 3);
  buf.u8(0x44); buf.u8(0x0F); buf.u8(0xB6); buf.u8(0x03);
  for (size_t i = 0; i < n; i++) {
    const int32_t v = static_cast<int32_t>(metas[i].value);
    const int32_t m = static_cast<int32_t>(metas[i].move);
    // imul eax, r8d, imm32
    buf.u8(0x41); buf.u8(0x69); buf.u8(0xC0); buf.u32(static_cast<uint32_t>(v));
    // add [rbx + disp32], al   --  opcode 00 /r, ModRM 83 = [rbx]+disp32, reg=al
    buf.u8(0x00); buf.u8(0x83); buf.u32(static_cast<uint32_t>(m));
  }
  buf.u8(0xC6); buf.u8(0x03); buf.u8(0x00);
  EMIT_END();
}

// Function epilogue.
//   add rsp, 8   -> 48 83 C4 08   (4)
//   pop rbx      -> 5B            (1)
//   ret          -> C3            (1)
void emitEpilogue(Buf& buf) {
  EMIT_BEGIN(kEpilogueSize);
  buf.u8(0x48); buf.u8(0x83); buf.u8(0xC4); buf.u8(0x08);
  buf.u8(0x5B);
  buf.u8(0xC3);
  EMIT_END();
}

// Computes the offset table and total body size (excluding prologue).
//   offsets[i] = byte offset (within body) where instruction i's code starts.
//   offsets[N] = total body size.
std::vector<size_t> computeOffsets(const Instruction* begin, const Instruction* end) {
  const size_t n = static_cast<size_t>(end - begin);
  std::vector<size_t> offsets(n + 1);
  size_t pos = 0;
  for (size_t i = 0; i < n; i++) {
    offsets[i] = pos;
    const auto& ins = begin[i];
    if (ins.move != 0 && ins.type != DATA_TRANSFER_META) {
      pos += kPreMoveSize;
    }
    if (ins.type == INSTRUCTION_POINTER_SET_IF_NOT_ZERO) {
      pos += closeBracketSize(isScanLoop(i, ins));
    } else {
      pos += bodySize(ins);
    }
  }
  offsets[n] = pos;
  return offsets;
}

using JitFn = void (*)(int8_t*);

// Allocate an executable page with the prologue, body bytes, and make it
// runnable via mmap PROT_READ|PROT_EXEC after the writable copy is done.
class JitCode {
 public:
  JitCode(const uint8_t* prologue, size_t prologue_size,
          const uint8_t* body, size_t body_size) {
    size_ = prologue_size + body_size;
    // mmap with RW so we can write; later mprotect to R|X.
    page_ = mmap(nullptr, size_, PROT_READ | PROT_WRITE,
                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (page_ == MAP_FAILED) {
      throw std::runtime_error("mmap failed for JIT buffer");
    }
    auto* dst = static_cast<uint8_t*>(page_);
    std::memcpy(dst, prologue, prologue_size);
    std::memcpy(dst + prologue_size, body, body_size);
    if (mprotect(page_, size_, PROT_READ | PROT_EXEC) != 0) {
      munmap(page_, size_);
      throw std::runtime_error("mprotect(R|X) failed for JIT buffer");
    }
  }

  ~JitCode() { if (page_) munmap(page_, size_); }

  JitCode(const JitCode&) = delete;
  JitCode& operator=(const JitCode&) = delete;

  JitFn fn() const { return reinterpret_cast<JitFn>(page_); }

 private:
  void* page_ = nullptr;
  size_t size_ = 0;
};

}  // namespace

// ---------------------------------------------------------------------------
// execute(): parse-time Instruction[] -> JIT -> run.
// ---------------------------------------------------------------------------

void execute(const Instruction* begin, const Instruction* end) {
  const size_t n = static_cast<size_t>(end - begin);
  const auto offsets = computeOffsets(begin, end);
  const size_t body_size = offsets[n];

  Buf buf(body_size);

  for (size_t i = 0; i < n; i++) {
    const auto& ins = begin[i];

    // Sanity: emission position tracks the offset table.
    if (buf.pos() != offsets[i]) {
      throw std::runtime_error("JIT offset drift before op emission");
    }

    // Pre-move: `add rbx, move` before body (mirrors the NEXT block in the
    // interpreter). DATA_TRANSFER_META repurposes .move as a tape offset,
    // not a pointer delta, so skip the prefix for it.
    if (ins.move != 0 && ins.type != DATA_TRANSFER_META) {
      emitPreMove(buf, ins.move);
    }

    switch (ins.type) {
      case NOOP:
        break;
      case DONE:
        emitEpilogue(buf);
        break;
      case DATA_ADD:
        emitDataAdd(buf, ins.value);
        break;
      case DATA_PRINT:
        emitDataPrint(buf, ins.value);
        break;
      case DATA_SET_FROM_INPUT:
        emitDataInput(buf, ins.value);
        break;
      case INSTRUCTION_POINTER_SET_IF_ZERO: {
        // Jump target: start of instruction at index `ins.value` (parser stores
        // the index AFTER the matching `]`; in practice this means jumping over
        // the closed loop).
        const size_t target = offsets[static_cast<size_t>(ins.value)];
        const size_t here_end = buf.pos() + 9;
        const int64_t rel = static_cast<int64_t>(target) - static_cast<int64_t>(here_end);
        emitOpen(buf, static_cast<int32_t>(rel));
        break;
      }
      case INSTRUCTION_POINTER_SET_IF_NOT_ZERO: {
        if (isScanLoop(i, ins)) {
          emitCloseScan(buf, ins.move != 0 ? ins.move : 1);
          // Note: `move` is typically nonzero for scan loops (the scan step).
          // If zero, the loop would be an infinite loop in the source BF; we
          // substitute 1 to avoid a degenerate rel32 zero which would match
          // a no-op. This path should not occur for well-formed programs.
        } else {
          const size_t target = offsets[static_cast<size_t>(ins.value)];
          const size_t here_end = buf.pos() + 9;
          const int64_t rel = static_cast<int64_t>(target) - static_cast<int64_t>(here_end);
          emitCloseNormal(buf, static_cast<int32_t>(rel));
        }
        break;
      }
      case DATA_TRANSFER: {
        const size_t meta_count = static_cast<size_t>(ins.value);
        if (i + meta_count >= n) {
          throw std::runtime_error("DATA_TRANSFER META overrun");
        }
        emitDataTransfer(buf, meta_count, &begin[i + 1]);
        break;
      }
      case DATA_TRANSFER_META:
        // Body emitted as part of the preceding DATA_TRANSFER.
        break;
      case DATA_POINTER_ADD:
        throw std::runtime_error("DATA_POINTER_ADD should have been squashed by parse.cpp");
    }
  }

  if (buf.pos() != body_size) {
    throw std::runtime_error("JIT body size mismatch after emission");
  }

  // Prologue:
  //   push rbx            -> 53
  //   mov  rbx, rdi       -> 48 89 FB
  //   sub  rsp, 8         -> 48 83 EC 08   (keeps rsp 16-byte aligned for call)
  constexpr uint8_t kPrologue[] = {
      0x53,
      0x48, 0x89, 0xFB,
      0x48, 0x83, 0xEC, 0x08,
  };

  JitCode code(kPrologue, sizeof(kPrologue), buf.data().data(), buf.data().size());

  int8_t datas[30000] = {0};
  code.fn()(&datas[0]);
}

}  // namespace brainfuck
