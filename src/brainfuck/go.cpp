#include "go.h"

#include "execute.h"
#include "file.h"
#include "parse.h"

namespace brainfuck {

Result go(const std::string filename) {
  const auto readResult = read(filename);
  if (!readResult) {
    return Result::PROGRAM_NOT_FOUND;
  }

  const auto parseResult1 = parse(*readResult);
  if (!parseResult1) {
    return Result::UNMATCHED_BRACE;
  }

  write(
      "instructions.bin", reinterpret_cast<const char*>(parseResult1->data()),
      static_cast<std::streamsize>(sizeof(Instruction) * parseResult1->size()));
  const auto parseResult2 = read("instructions.bin");
  const Instruction* parseResult = reinterpret_cast<const Instruction*>(parseResult2->data() + sizeof(uint64_t));

  execute(parseResult, parseResult + parseResult1->size());

  return Result::DONE;
}

}  // namespace brainfuck
