#include "execute.h"

#include <iostream>
#include <stdexcept>

namespace brainfuck {

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

void setupInstructionAddresses(const Instruction* begin, const Instruction* end, const void* jumpTable[]) {
  Instruction* current = const_cast<Instruction*>(begin);
  while (current < end) {
    if (current->type == DATA_POINTER_ADD) {
      throw std::runtime_error("Un-implemented instruction type");
    }
    if (current->type == DATA_TRANSFER) {
      current->next = current + current->value;
    }
    if (current->type == INSTRUCTION_POINTER_SET_IF_NOT_ZERO || current->type == INSTRUCTION_POINTER_SET_IF_ZERO) {
      current->next = const_cast<Instruction*>(begin + current->value);
    }
    current->jump = const_cast<void*>(jumpTable[current->type]);
    current++;
  }
}

}  // namespace brainfuck

// ---------------------------------------------------------------------------
// Bridge between the C++ interpreter and the hand-written assembly loop in
// execute_asm.S. The assembly exports:
//   - brainfuck_jump_table : handler addresses indexed by Type
//   - brainfuck_execute_asm(Instruction* begin, int8_t* data)
// and calls back into the two shims below for I/O.
// ---------------------------------------------------------------------------
extern "C" {

extern const void* brainfuck_jump_table[];
void brainfuck_execute_asm(const brainfuck::Instruction* begin, int8_t* data);

void bf_output_c(int8_t c, int64_t times) {
  brainfuck::output(c, times);
}

void bf_input_c(int8_t* p, int64_t times) {
  brainfuck::input(p, times);
}

}  // extern "C"

namespace brainfuck {

void execute(const Instruction* begin, const Instruction* end) {
  setupInstructionAddresses(begin, end, brainfuck_jump_table);

  int8_t datas[30000] = {0};
  brainfuck_execute_asm(begin, &datas[0]);
}

}  // namespace brainfuck
