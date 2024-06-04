#include "execute.h"

#include <iostream>
#include <limits>

namespace brainfuck {

int zeroOrOne(unsigned char value) {
  if (value == 0) {
    return 0;
  }
  return 1;
}

// custom way to send this type to an output stream
std::ostream& operator<<(std::ostream& os, const Instruction& instruction) {
  return os << "+" << instruction.mutate_data << " >" << instruction.mutate_data_pointer << " if0"
            << instruction.mutate_instruction_pointer_if[0] << " if!0" << instruction.mutate_instruction_pointer_if[1];
}

void execute(std::vector<Instruction> instructions) {
  const auto instruction_count = instructions.size();

  int instruction_pointer = 0;

  unsigned short data_pointer = 0;
  unsigned char memory[std::numeric_limits<unsigned short>::max()] = {0};

  while (instruction_pointer < instruction_count) {
    const auto instruction = instructions[instruction_pointer];

    /* std::cout << "[" << instruction_pointer << "] " << instruction << " (" << (int)memory[data_pointer] << ")\n"; */

    instruction_pointer += instruction.mutate_instruction_pointer_if[zeroOrOne(memory[data_pointer])];
    memory[data_pointer] += instruction.mutate_data;
    data_pointer += instruction.mutate_data_pointer;
    if (instruction.io == WRITE) {
      std::cout << (char)memory[data_pointer];
    } else if (instruction.io == READ) {
      std::cin >> memory[data_pointer];
    }
  }
};

}  // namespace brainfuck
