#pragma once

#include <string>
#include <vector>

namespace brainfuck {

enum IO { READ, WRITE };
struct Instruction {
  int mutate_data = 0;
  int mutate_data_pointer = 0;
  int mutate_instruction_pointer_if[2] = {
      1,  // if zero
      1,  // if not zero
  };
  IO io;
};

std::vector<Instruction> parse(std::vector<char> full);

}  // namespace brainfuck
