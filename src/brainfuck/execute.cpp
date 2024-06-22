#include "execute.h"

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
    instruction.jump = jumpTable[instruction.type - 1];
    if (instruction.type == INSTRUCTION_POINTER_SET_IF_NOT_ZERO) {
      instruction.value = reinterpret_cast<Value>(&instructions[static_cast<size_t>(instruction.value)]);
    } else if (instruction.type == INSTRUCTION_POINTER_SET_IF_ZERO) {
      instruction.value = reinterpret_cast<Value>(&instructions[static_cast<size_t>(instruction.value)]);
    }
  }

  goto*(instruction->jump);

DATA_POINTER_ADD: {
  data += instruction->value;

  instruction++;
  goto*(instruction->jump);
}

DATA_ADD: {
  *data += instruction->value;

  instruction++;
  goto*(instruction->jump);
}

INSTRUCTION_POINTER_SET_IF_NOT_ZERO: {
  if ((*data) % 256 != 0) {
    instruction = reinterpret_cast<Instruction*>(instruction->value);
    goto*(instruction->jump);
  }

  instruction++;
  goto*(instruction->jump);
}

DATA_TRANSFER: {
  throw std::runtime_error("missed!");
  *(data + instruction->value) += *data;
  *data = 0;

  instruction++;
  goto*(instruction->jump);
}

DATA_MULTIPLY: {
  throw std::runtime_error("missed!");
  instruction++;
  goto*(instruction->jump);
}

DATA_SET: {
  *data = instruction->value;

  instruction++;
  goto*(instruction->jump);
}

INSTRUCTION_POINTER_SET_IF_ZERO: {
  if ((*data) % 256 == 0) {
    instruction = reinterpret_cast<Instruction*>(instruction->value);
    goto*(instruction->jump);
  }

  instruction++;
  goto*(instruction->jump);
}

DATA_POINTER_ADD_WHILE_NOT_ZERO: {
  while (*data % 256 != 0) {
    data += instruction->value;
  }

  instruction++;
  goto*(instruction->jump);
}

DATA_PRINT: {
  for (int i = 0; i < instruction->value; i++) {
    std::cout << static_cast<char>(*data % 256);
  }

  instruction++;
  goto*(instruction->jump);
}

DATA_SET_FROM_INPUT: {
  for (int i = 0; i < instruction->value; i++) {
    char input;
    std::cin >> std::noskipws >> input;
    if (!std::cin.eof()) {
      *data = input;
    }
  }

  instruction++;
  goto*(instruction->jump);
}

DATA_MULTIPLY_AND_DIVIDE: {
  throw std::runtime_error("missed!");
  instruction++;
  goto*(instruction->jump);
}

DONE: { return Error::NONE; }
};

}  // namespace brainfuck
