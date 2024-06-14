#pragma once

#include <concepts>
#include <vector>
#include "error.h"
#include "parse.h"

namespace brainfuck {

constexpr uint64_t ITERATION_LIMIT = 10000000000;
constexpr bool INSTRUCTION_COUNTS = true;

Error execute(ByteCode instructions) {
  const auto instruction_count = instructions.size();
  size_t instruction_pointer = 0;
  int32_t data_pointer = 0;
  char data[30000] = {0};

  uint64_t iteration;
  if constexpr (ITERATION_LIMIT > 0) {
    iteration = 0;
  }

  std::vector<uint32_t> instruction_run_count;
  if constexpr (INSTRUCTION_COUNTS) {
    instruction_run_count.resize(instruction_count, 0);
  }

  while (instruction_pointer < instruction_count) {
    if constexpr (ITERATION_LIMIT > 0) {
      if (++iteration > ITERATION_LIMIT) {
        std::cerr << "Exceeded maximum iterations (" << ITERATION_LIMIT << " iteration limit)" << std::endl;
        return Error::REACHED_INSRUCTION_LIMIT;
      }
    }
    if constexpr (INSTRUCTION_COUNTS) {
      instruction_run_count[instruction_pointer]++;
    }

    const Instruction instruction = instructions[instruction_pointer];

    const int32_t offset_data_pointer = data_pointer + instruction.offset;
    switch (instruction.type) {
      case DATA_ADD:
        data[offset_data_pointer] += static_cast<char>(instruction.value);
        break;
      case DATA_SET:
        data[offset_data_pointer] = static_cast<char>(instruction.value);
        break;
      case DATA_POINTER_ADD:
        data_pointer += static_cast<size_t>(instruction.offset);
        break;
      case INSTRUCTION_POINTER_SET_IF_ZERO:
        if (data[offset_data_pointer] == 0) {
          instruction_pointer = static_cast<size_t>(instruction.value);
          continue;
        }
        break;
      case INSTRUCTION_POINTER_SET_IF_NOT_ZERO:
        if (data[offset_data_pointer] != 0) {
          instruction_pointer = static_cast<size_t>(instruction.value);
          continue;
        }
        break;
      case DATA_PRINT:
        for (int i = 0; i < instruction.value; i++) {
          std::cout << (char)data[offset_data_pointer];
        }
        break;
      case DATA_SET_FROM_INPUT:
        for (int i = 0; i < instruction.value; i++) {
          char input;
          std::cin >> std::noskipws >> input;
          if (!std::cin.eof()) {
            data[offset_data_pointer] = input;
          }
        }
        break;
      case NOOP:
        break;
    }

    instruction_pointer++;
  }

  return Error::NONE;
};

}  // namespace brainfuck
