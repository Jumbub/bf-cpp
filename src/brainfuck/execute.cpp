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

  static void* jumpTable[] = {
      &&NOOP,
      &&DONE,
      &&DATA_ADD,
      &&DATA_SET,
      &&DATA_TRANSFER,
      &&DATA_MULTIPLY,
      &&DATA_MULTIPLY_AND_DIVIDE,
      &&DATA_SET_FROM_INPUT,
      &&DATA_PRINT,
      &&DATA_POINTER_ADD,
      &&DATA_POINTER_ADD_WHILE_NOT_ZERO,
      &&INSTRUCTION_POINTER_SET_IF_ZERO,
      &&INSTRUCTION_POINTER_SET_IF_NOT_ZERO,
  };

  goto* jumpTable[instruction->type];

DATA_POINTER_ADD:
  data_pointer += instruction->offset;
  goto AFTER;

INSTRUCTION_POINTER_SET_IF_NOT_ZERO: {
  const auto offset_data_pointer = data_pointer + instruction->offset;
  if (data[offset_data_pointer] % 256 != 0) {
    instruction = &instructions[static_cast<size_t>(instruction->value)];
    goto* jumpTable[instruction->type];
  }
  goto AFTER;
}

DATA_TRANSFER: {
  const auto offset_data_pointer = data_pointer + instruction->offset;
  data[offset_data_pointer + instruction->value] += data[offset_data_pointer];
  data[offset_data_pointer] = 0;
  goto AFTER;
}

DATA_ADD: {
  data[data_pointer + instruction->offset] += instruction->value;
  goto AFTER;
}

DATA_MULTIPLY: {
  const auto offset_data_pointer = data_pointer + instruction->offset;
  const auto outputs = instruction->value;
  for (auto i = 0; i < outputs; i++) {
    instruction++;
    data[offset_data_pointer + instruction->offset] += instruction->value * data[offset_data_pointer];
  }
  data[offset_data_pointer] = 0;
  goto AFTER;
}

DATA_SET: {
  const auto offset_data_pointer = data_pointer + instruction->offset;
  data[offset_data_pointer] = instruction->value;
  goto AFTER;
}

INSTRUCTION_POINTER_SET_IF_ZERO: {
  const auto offset_data_pointer = data_pointer + instruction->offset;
  if (data[offset_data_pointer] % 256 == 0) {
    instruction = &instructions[static_cast<size_t>(instruction->value)];
    goto* jumpTable[instruction->type];
  }
  goto AFTER;
}

DATA_POINTER_ADD_WHILE_NOT_ZERO:
  while (data[data_pointer + instruction->offset] % 256 != 0) {
    data_pointer += instruction->value;
  }
  goto AFTER;

DATA_PRINT: {
  const auto offset_data_pointer = data_pointer + instruction->offset;
  for (int i = 0; i < instruction->value; i++) {
    std::cout << static_cast<char>(data[offset_data_pointer] % 256);
  }
  goto AFTER;
}

DATA_SET_FROM_INPUT: {
  const auto offset_data_pointer = data_pointer + instruction->offset;
  for (int i = 0; i < instruction->value; i++) {
    char input;
    std::cin >> std::noskipws >> input;
    if (!std::cin.eof()) {
      data[offset_data_pointer] = input;
    }
  }
  goto AFTER;
}

DATA_MULTIPLY_AND_DIVIDE: {
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
  goto AFTER;
}

DONE:
  // debug.done();
  return Error::NONE;

NOOP:
  goto AFTER;

AFTER:
  // debug.trackInstruction(static_cast<size_t>(instruction - instructions.data()));
  instruction++;
  goto* jumpTable[instruction->type];
};

}  // namespace brainfuck
