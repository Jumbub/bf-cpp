#include "execute.h"

#include <algorithm>
#include <iostream>
#include <vector>

namespace brainfuck {

static void output(int64_t output, Value times) {
  for (int i = 0; i < times; i++) {
    std::cout << static_cast<char>(output % 256);
  }
}

static void input(int64_t* character, Value times) {
  for (int i = 0; i < times; i++) {
    char input;
    std::cin >> std::noskipws >> input;
    if (!std::cin.eof()) {
      *character = input;
    }
  }
}

void execute(std::vector<Instruction> instructions) {
  int64_t datas[30000] = {0};
  int64_t* data = &datas[0];
  Instruction* instruction = &instructions[0];

  static void* jumpTable[] = {
      &&NEXT,                                 // foo
      &&DONE,                                 // EOF
      &&DATA_ADD,                             // + // -
      &&DATA_SET_FROM_INPUT,                  // ,
      &&DATA_PRINT,                           // .
      &&DATA_POINTER_ADD,                     // > // <
      &&INSTRUCTION_POINTER_SET_IF_ZERO,      // [
      &&INSTRUCTION_POINTER_SET_IF_NOT_ZERO,  // ]
  };

  for (auto& instruction : instructions) {
    if (instruction.type == INSTRUCTION_POINTER_SET_IF_NOT_ZERO ||
        instruction.type == INSTRUCTION_POINTER_SET_IF_ZERO) {
      instruction.next = &instructions[static_cast<size_t>(instruction.value)];
    }
    instruction.jump = jumpTable[instruction.type];
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
    instruction = instruction->next;
    goto*(instruction->jump);
  }

  goto NEXT;
}

INSTRUCTION_POINTER_SET_IF_ZERO: {
  data += instruction->offset;

  if ((*data & 255) == 0) {
    instruction = instruction->next;
    goto*(instruction->jump);
  }

  goto NEXT;
}

DATA_PRINT: {
  output(*(data + instruction->offset), instruction->value);

  goto NEXT;
}

DATA_SET_FROM_INPUT: {
  input((data + instruction->offset), instruction->value);

  goto NEXT;
}

DONE:
};

}  // namespace brainfuck
