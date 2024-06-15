#pragma once

#include <concepts>
#include <vector>
#include "error.h"
#include "parse.h"

namespace brainfuck {

constexpr uint64_t ITERATION_LIMIT = 10000000000;
constexpr bool INSTRUCTION_COUNTS = false;

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
        return Error::REACHED_INSTRUCTION_LIMIT;
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
        data_pointer += instruction.offset;
        break;
      case DATA_POINTER_ADD_WHILE_NOT_ZERO:
        while (data[data_pointer + instruction.offset] != 0) {
          data_pointer += instruction.value;
        }
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
          std::cout << static_cast<char>(data[offset_data_pointer]);
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

  if constexpr (INSTRUCTION_COUNTS) {
    for (size_t i = 0; i < instructions.size(); i++) {
      const auto instruction = instructions[i];
      std::cout << std::format(
          "{} {:04} [{:04}] ! {:010}\n", static_cast<char>(instruction.type), instruction.value, instruction.offset,
          instruction_run_count[i]);
    }
  }

  return Error::NONE;
};

}  // namespace brainfuck
