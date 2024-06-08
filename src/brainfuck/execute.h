#pragma once

#include <concepts>
#include <vector>
#include "parse.h"

namespace brainfuck {

enum EOFBehaviour { SET_ZERO, SET_NEGATIVE_ONE, EOF_NOOP };
enum DataPointerOverflowBehaviour { UNDEFINED };
struct Config {
  const uint64_t data_size = 30000;
  const EOFBehaviour eof_behaviour = EOF_NOOP;
  const DataPointerOverflowBehaviour data_pointer_overflow_behaviour = UNDEFINED;
};

using DefaultData = char;
using DefaultDataPointer = uint64_t;
using DefaultInstructionPointer = uint64_t;
constexpr Config DefaultConfig{
    Config{.data_size = 30000, .eof_behaviour = EOF_NOOP, .data_pointer_overflow_behaviour = UNDEFINED}};

template <
    Config config = DefaultConfig,
    std::integral Data = DefaultData,
    std::integral DataPointer = DefaultDataPointer,
    std::integral InstructionPointer = DefaultInstructionPointer>
int execute(Instructions instructions) {
  static_assert(config.data_size <= std::numeric_limits<DataPointer>::max(), "Exceeded maximum allowed memory");
  constexpr auto PROGRAM_SIZE_LIMIT = std::numeric_limits<InstructionPointer>::max();
  if (instructions.size() > PROGRAM_SIZE_LIMIT) {
    std::cerr << std::format("Exceeded maximum program size ({} character limit)\n", PROGRAM_SIZE_LIMIT);
    return EXIT_FAILURE;
  }

  const auto instruction_count = instructions.size();
  InstructionPointer instruction_pointer = 0;
  DataPointer data_pointer = 0;
  Data data[config.data_size] = {0};

  while (instruction_pointer < instruction_count) {
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
        if (std::cin.eof()) {
          if constexpr (config.eof_behaviour == EOFBehaviour::SET_ZERO) {
            data[data_pointer] = 0;
          } else if constexpr (config.eof_behaviour == EOFBehaviour::SET_NEGATIVE_ONE) {
            data[data_pointer] = -1;
          }
        } else {
          std::cin >> data[data_pointer];
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
