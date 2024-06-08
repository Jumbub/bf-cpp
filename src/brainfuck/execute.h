#pragma once

#include <concepts>
#include <vector>
#include "parse.h"

namespace brainfuck {

enum class EOFBehaviour { SET_ZERO, SET_NEGATIVE_ONE, NOOP };
enum class DataPointerOverflowBehaviour { UNDEFINED };

template <
    uint64_t DATA_SIZE,
    uint64_t ITERATION_LIMIT,
    EOFBehaviour EOF_BEHAVIOUR,
    DataPointerOverflowBehaviour DATA_POINTER_OVERFLOW_BEHAVIOUR,
    std::integral Data,
    std::integral DataPointer,
    std::integral InstructionPointer>
int execute(Instructions instructions) {
  static_assert(DATA_SIZE <= std::numeric_limits<DataPointer>::max(), "Exceeded maximum allowed memory");
  constexpr auto PROGRAM_SIZE_LIMIT = std::numeric_limits<InstructionPointer>::max();
  if (instructions.size() > PROGRAM_SIZE_LIMIT) {
    std::cerr << "Exceeded maximum program size (" << PROGRAM_SIZE_LIMIT << " character limit)\n";
    return EXIT_FAILURE;
  }

  const auto instruction_count = instructions.size();
  InstructionPointer instruction_pointer = 0;
  DataPointer data_pointer = 0;
  Data data[DATA_SIZE] = {0};
  uint64_t iteration = 0;

  while (instruction_pointer < instruction_count) {
    if constexpr (ITERATION_LIMIT > 0) {
      if (++iteration > ITERATION_LIMIT) {
        std::cerr << "Exceeded maximum iterations (" << ITERATION_LIMIT << " iteration limit)" << std::endl;
        break;
      }
    }

    const auto instruction = instructions[instruction_pointer];

    switch (instruction.type) {
      case MUTATE_DATA:
        data[data_pointer] += instruction.value;
        instruction_pointer++;
        break;
      case MUTATE_DATA_POINTER:
        data_pointer += instruction.value;
        instruction_pointer++;
        break;
      case MUTATE_INSTRUCTION_POINTER_IF_ZERO:
        if (data[data_pointer] == 0) {
          instruction_pointer = instruction.value;
        } else {
          instruction_pointer++;
        }
        break;
      case MUTATE_INSTRUCTION_POINTER_IF_NOT_ZERO:
        if (data[data_pointer] != 0) {
          instruction_pointer = instruction.value;
        } else {
          instruction_pointer++;
        }
        break;
      case WRITE:
        std::cout << (char)data[data_pointer];
        instruction_pointer++;
        break;
      case READ:
        std::cin >> std::noskipws >> data[data_pointer];
        if (std::cin.eof()) {
          if constexpr (EOF_BEHAVIOUR == EOFBehaviour::SET_ZERO) {
            data[data_pointer] = 0;
          } else if constexpr (EOF_BEHAVIOUR == EOFBehaviour::SET_NEGATIVE_ONE) {
            data[data_pointer] = -1;
          }
        }
        instruction_pointer++;
        break;
      case NOOP:
        instruction_pointer++;
        break;
    }
  }

  return EXIT_SUCCESS;
};

}  // namespace brainfuck
