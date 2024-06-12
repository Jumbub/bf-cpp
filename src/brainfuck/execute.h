#pragma once

#include <concepts>
#include <vector>
#include "error.h"
#include "parse.h"

namespace brainfuck {

constexpr uint64_t ITERATION_LIMIT = 10000000000;

Error execute(ByteCode instructions) {
  const auto instruction_count = instructions.size();
  size_t instruction_pointer = 0;
  size_t data_pointer = 0;
  char data[30000] = {0};

  uint64_t iteration;
  if constexpr (ITERATION_LIMIT > 0) {
    iteration = 0;
  }

  while (instruction_pointer < instruction_count) {
    if constexpr (ITERATION_LIMIT > 0) {
      if (++iteration > ITERATION_LIMIT) {
        std::cerr << "Exceeded maximum iterations (" << ITERATION_LIMIT << " iteration limit)" << std::endl;
        return Error::REACHED_INSRUCTION_LIMIT;
      }
    }

    const Instruction instruction = instructions[instruction_pointer];

    switch (instruction.type) {
      case MUTATE_DATA:
        data[data_pointer] += static_cast<char>(instruction.value);
        instruction_pointer++;
        break;
      case SET:
        data[data_pointer] = static_cast<char>(instruction.value);
        instruction_pointer++;
        break;
      case MUTATE_DATA_POINTER:
        data_pointer += static_cast<size_t>(instruction.value);
        instruction_pointer++;
        break;
      case MUTATE_INSTRUCTION_POINTER_IF_ZERO:
        if (data[data_pointer] == 0) {
          instruction_pointer = static_cast<size_t>(instruction.value);
        } else {
          instruction_pointer++;
        }
        break;
      case MUTATE_INSTRUCTION_POINTER_IF_NOT_ZERO:
        if (data[data_pointer] != 0) {
          instruction_pointer = static_cast<size_t>(instruction.value);
        } else {
          instruction_pointer++;
        }
        break;
      case WRITE:
        std::cout << (char)data[data_pointer];
        instruction_pointer++;
        break;
      case READ:
        char input;
        std::cin >> std::noskipws >> input;
        if (!std::cin.eof()) {
          data[data_pointer] = input;
        }
        instruction_pointer++;
        break;
      case NOOP:
        instruction_pointer++;
        break;
    }
  }

  return Error::NONE;
};

}  // namespace brainfuck
