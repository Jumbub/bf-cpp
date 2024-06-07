#include "execute.h"

#include <iostream>
#include <limits>

namespace brainfuck {

void execute(std::vector<Instruction> instructions) {
  const auto instruction_count = instructions.size();

  int instruction_pointer = 0;

  unsigned short data_pointer = 0;
  unsigned char memory[std::numeric_limits<unsigned short>::max()] = {0};

  while (instruction_pointer < instruction_count) {
    const auto instruction = instructions[instruction_pointer];

    switch (instruction.type) {
      case MUTATE_DATA:
        memory[data_pointer] += instruction.value;
        instruction_pointer++;
        break;
      case MUTATE_DATA_POINTER:
        data_pointer += instruction.value;
        instruction_pointer++;
        break;
      case MUTATE_INSTRUCTION_POINTER_IF_ZERO:
        if (memory[data_pointer] == 0) {
          instruction_pointer = instruction.value;
        }
        break;
      case MUTATE_INSTRUCTION_POINTER_IF_NOT_ZERO:
        if (memory[data_pointer] != 0) {
          instruction_pointer = instruction.value;
        }
        break;
      case WRITE:
        std::cout << (char)memory[data_pointer];
        instruction_pointer++;
        break;
      case READ:
        std::cin >> memory[data_pointer];
        instruction_pointer++;
        break;
      case NOOP:
        instruction_pointer++;
        break;
    }
  }
};

}  // namespace brainfuck
