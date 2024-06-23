#include "execute.h"

#include <algorithm>
#include <iostream>
#include <vector>

namespace brainfuck {

Error execute(ByteCode instructions) {
  int64_t datas[30000] = {0};
  int64_t* data = &datas[0];
  Instruction* instruction = &instructions[0];

  static void* jumpTable[] = {
      &&DONE,
      &&DATA_ADD,
      &&DATA_SET,
      &&DATA_RESET,
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
    if (instruction.type == NOOP) {
      throw std::runtime_error("Parser failed to strip NOOP instructions");
    }
    if (instruction.type == INSTRUCTION_POINTER_SET_IF_NOT_ZERO) {
      instruction.value = reinterpret_cast<Value>(&instructions[static_cast<size_t>(instruction.value)]);
    } else if (instruction.type == INSTRUCTION_POINTER_SET_IF_ZERO) {
      instruction.value = reinterpret_cast<Value>(&instructions[static_cast<size_t>(instruction.value)]);
    }
    instruction.jump = jumpTable[instruction.type - 1];
  }

  goto*(instruction->jump);

NEXT: {
  instruction++;
  goto*(instruction->jump);
}

DATA_POINTER_ADD: {
  data += instruction->offset;

  goto NEXT;
}

DATA_ADD: {
  *(data + instruction->offset) += instruction->value;

  goto NEXT;
}

INSTRUCTION_POINTER_SET_IF_NOT_ZERO: {
  data += instruction->offset;

  if ((*data & 255) != 0) {
    instruction = reinterpret_cast<Instruction*>(instruction->value);
    goto*(instruction->jump);
  }

  goto NEXT;
}

DATA_TRANSFER: {
  const auto offset_data_pointer = data + instruction->offset;
  *(offset_data_pointer + instruction->value) += *(offset_data_pointer);
  *(offset_data_pointer) = 0;

  goto NEXT;
}

DATA_MULTIPLY: {
  const auto offset_data_pointer = data + instruction->offset;
  const auto multiplier = *offset_data_pointer;
  *(offset_data_pointer) = 0;

  const auto lastInstruction = instruction + instruction->value;
  std::for_each(instruction + 1, lastInstruction + 1, [&offset_data_pointer, &multiplier](const auto& instruction) {
    *(offset_data_pointer + instruction.offset) += instruction.value * multiplier;
  });
  instruction = lastInstruction;

  goto NEXT;
}

DATA_SET: {
  *(data + instruction->offset) = instruction->value;

  goto NEXT;
}

DATA_RESET: {
  *(data + instruction->offset) = 0;
  *(data + instruction->value) = 0;

  goto NEXT;
}

INSTRUCTION_POINTER_SET_IF_ZERO: {
  data += instruction->offset;

  if ((*data & 255) == 0) {
    instruction = reinterpret_cast<Instruction*>(instruction->value);
    goto*(instruction->jump);
  }

  goto NEXT;
}

DATA_POINTER_ADD_WHILE_NOT_ZERO: {
  auto offset_data_pointer = data + instruction->offset;
  while ((*offset_data_pointer & 255) != 0) {
    offset_data_pointer += instruction->value;
  }
  data = offset_data_pointer - instruction->offset;

  goto NEXT;
}

DATA_PRINT: {
  const auto offset_data_pointer = data + instruction->offset;
  for (int i = 0; i < instruction->value; i++) {
    std::cout << static_cast<char>((*offset_data_pointer) % 256);
  }

  goto NEXT;
}

DATA_SET_FROM_INPUT: {
  const auto offset_data_pointer = data + instruction->offset;
  for (int i = 0; i < instruction->value; i++) {
    char input;
    std::cin >> std::noskipws >> input;
    if (!std::cin.eof()) {
      *offset_data_pointer = input;
    }
  }

  goto NEXT;
}

DATA_MULTIPLY_AND_DIVIDE: {
  const auto offset_data_pointer = data + instruction->offset;
  const auto outputs = instruction->value;
  instruction++;
  Value iterations = 0;
  while ((*offset_data_pointer & 255) != 0) {
    *offset_data_pointer += instruction->value;
    iterations++;
  }

  const auto lastInstruction = instruction + outputs;
  while (instruction != lastInstruction) {
    instruction++;
    *(offset_data_pointer + instruction->offset) += instruction->value * iterations;
  }

  goto NEXT;
}

DONE: { return Error::NONE; }
};

}  // namespace brainfuck
