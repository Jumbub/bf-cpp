#include "execute.h"

#include <iostream>
#include <vector>
// #include "debug.h"

namespace brainfuck {

Error execute(ByteCode instructions) {
  int64_t data_pointer = 0;
  int64_t data[30000] = {0};
  Instruction* instruction = &instructions[0];

  // Debug debug{instructions};

next:
  // debug.trackInstruction(static_cast<size_t>(instruction - instructions.data()));
  switch (instruction->type) {
    case DATA_POINTER_ADD:
      data_pointer += instruction->offset;
      break;
    case INSTRUCTION_POINTER_SET_IF_NOT_ZERO: {
      const auto offset_data_pointer = data_pointer + instruction->offset;
      if (data[offset_data_pointer] % 256 != 0) {
        instruction = &instructions[static_cast<size_t>(instruction->value)];
        goto next;
      }
      break;
    }
    case DATA_TRANSFER: {
      const auto offset_data_pointer = data_pointer + instruction->offset;
      data[offset_data_pointer + instruction->value] += data[offset_data_pointer];
      data[offset_data_pointer] = 0;
      break;
    }
    case DATA_ADD: {
      data[data_pointer + instruction->offset] += instruction->value;
      break;
    }
    case DATA_MULTIPLY: {
      const auto offset_data_pointer = data_pointer + instruction->offset;
      const auto outputs = instruction->value;
      for (auto i = 0; i < outputs; i++) {
        instruction++;
        data[offset_data_pointer + instruction->offset] += instruction->value * data[offset_data_pointer];
      }
      data[offset_data_pointer] = 0;
      break;
    }
    case DATA_SET: {
      const auto offset_data_pointer = data_pointer + instruction->offset;
      data[offset_data_pointer] = instruction->value;
      break;
    }
    case INSTRUCTION_POINTER_SET_IF_ZERO: {
      const auto offset_data_pointer = data_pointer + instruction->offset;
      if (data[offset_data_pointer] % 256 == 0) {
        instruction = &instructions[static_cast<size_t>(instruction->value)];
        goto next;
      }
      break;
    }
    case DATA_POINTER_ADD_WHILE_NOT_ZERO:
      while (data[data_pointer + instruction->offset] % 256 != 0) {
        data_pointer += instruction->value;
      }
      break;
    case DATA_PRINT: {
      const auto offset_data_pointer = data_pointer + instruction->offset;
      for (int i = 0; i < instruction->value; i++) {
        std::cout << static_cast<char>(data[offset_data_pointer] % 256);
      }
      break;
    }
    case DATA_SET_FROM_INPUT: {
      const auto offset_data_pointer = data_pointer + instruction->offset;
      for (int i = 0; i < instruction->value; i++) {
        char input;
        std::cin >> std::noskipws >> input;
        if (!std::cin.eof()) {
          data[offset_data_pointer] = input;
        }
      }
      break;
    }
    case DATA_MULTIPLY_AND_DIVIDE: {
      const auto offset_data_pointer = data_pointer + instruction->offset;
      const auto outputs = instruction->value;
      instruction++;
      Value iterations = 0;
      while (data[offset_data_pointer] % 256 != 0) {
        data[offset_data_pointer] += instruction->value;
        iterations++;
      }

      for (auto i = 0; i < outputs; i++) {
        instruction++;
        data[offset_data_pointer + instruction->offset] += instruction->value * iterations;
      }
      break;
    }
    case DONE:
      // debug.done();
      return Error::NONE;
    case NOOP:
      break;
    default:
      __builtin_unreachable();
  }

  instruction++;

  goto next;
};

}  // namespace brainfuck
