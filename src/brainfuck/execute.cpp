#include "execute.h"

#include <iostream>
#include <vector>
// #include "debug.h"

namespace brainfuck {

Error execute(ByteCode instructions) {
  int64_t data_offset = 0;
  int64_t datas[30000] = {0};
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

  for (auto& instruction : instructions) {
    instruction.jump = jumpTable[instruction.type];
    if (instruction.type == INSTRUCTION_POINTER_SET_IF_NOT_ZERO) {
      instruction.value = reinterpret_cast<Value>(&instructions[static_cast<size_t>(instruction.value)]);
    } else if (instruction.type == INSTRUCTION_POINTER_SET_IF_ZERO) {
      instruction.value = reinterpret_cast<Value>(&instructions[static_cast<size_t>(instruction.value)]);
    }
  }

  goto*(instruction->jump);

DATA_POINTER_ADD: {
  data_offset += instruction->offset;

  instruction++;
  goto*(instruction->jump);
}

DATA_ADD: {
  datas[data_offset + instruction->offset] += instruction->value;

  instruction++;
  goto*(instruction->jump);
}

INSTRUCTION_POINTER_SET_IF_NOT_ZERO: {
  if (datas[data_offset] % 256 != 0) {
    instruction = reinterpret_cast<Instruction*>(instruction->value);
    goto*(instruction->jump);
  }

  instruction++;
  goto*(instruction->jump);
}

DATA_TRANSFER: {
  const auto offset_data_pointer = data_offset + instruction->offset;
  datas[offset_data_pointer + instruction->value] += datas[offset_data_pointer];
  datas[offset_data_pointer] = 0;

  instruction++;
  goto*(instruction->jump);
}

DATA_MULTIPLY: {
  const auto offset_data_pointer = data_offset + instruction->offset;
  const auto lastInstruction = instruction + instruction->value;
  const auto multiplier = datas[offset_data_pointer];
  while (instruction != lastInstruction) {
    instruction++;
    datas[offset_data_pointer + instruction->offset] += instruction->value * multiplier;
  }
  datas[offset_data_pointer] = 0;

  instruction++;
  goto*(instruction->jump);
}

DATA_SET: {
  const auto offset_data_pointer = data_offset + instruction->offset;
  datas[offset_data_pointer] = instruction->value;

  instruction++;
  goto*(instruction->jump);
}

INSTRUCTION_POINTER_SET_IF_ZERO: {
  if (datas[data_offset] % 256 == 0) {
    instruction = reinterpret_cast<Instruction*>(instruction->value);
    goto*(instruction->jump);
  }

  instruction++;
  goto*(instruction->jump);
}

DATA_POINTER_ADD_WHILE_NOT_ZERO: {
  while (datas[data_offset + instruction->offset] % 256 != 0) {
    data_offset += instruction->value;
  }

  instruction++;
  goto*(instruction->jump);
}

DATA_PRINT: {
  const auto offset_data_pointer = data_offset + instruction->offset;
  for (int i = 0; i < instruction->value; i++) {
    std::cout << static_cast<char>(datas[offset_data_pointer] % 256);
  }

  instruction++;
  goto*(instruction->jump);
}

DATA_SET_FROM_INPUT: {
  const auto offset_data_pointer = data_offset + instruction->offset;
  for (int i = 0; i < instruction->value; i++) {
    char input;
    std::cin >> std::noskipws >> input;
    if (!std::cin.eof()) {
      datas[offset_data_pointer] = input;
    }
  }

  instruction++;
  goto*(instruction->jump);
}

DATA_MULTIPLY_AND_DIVIDE: {
  const auto offset_data_pointer = data_offset + instruction->offset;
  const auto outputs = instruction->value;
  instruction++;
  Value iterations = 0;
  while (datas[offset_data_pointer] % 256 != 0) {
    datas[offset_data_pointer] += instruction->value;
    iterations++;
  }

  const auto lastInstruction = instruction + outputs;
  while (instruction != lastInstruction) {
    instruction++;
    datas[offset_data_pointer + instruction->offset] += instruction->value * iterations;
  }

  instruction++;
  goto*(instruction->jump);
}

NOOP: {
  instruction++;
  goto*(instruction->jump);
}

DONE: {
  // debug.done();
  return Error::NONE;
}
};

}  // namespace brainfuck
