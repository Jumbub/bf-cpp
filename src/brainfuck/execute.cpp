#include "execute.h"

#include <iostream>

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

void setupInstructionAddresses(const Instruction* begin, const Instruction* end, const void* jumpTable[]) {
  Instruction* current = const_cast<Instruction*>(begin);
  while (current < end) {
    if (current->type == DATA_POINTER_ADD) {
      throw std::runtime_error("Un-implemented instruction type");
    }
    if (current->type == INSTRUCTION_POINTER_SET_IF_NOT_ZERO || current->type == INSTRUCTION_POINTER_SET_IF_ZERO) {
      current->next = const_cast<Instruction*>(begin + current->value);
    }
    current->jump = const_cast<void*>(jumpTable[current->type]);
    current++;
  }
}

void execute(const Instruction* begin, const Instruction* end) {
  const void* jumpTable[] = {
      &&NEXT,
      &&DONE,
      &&DATA_ADD,
      &&DATA_SET_FROM_INPUT,
      &&DATA_PRINT,
      nullptr,
      &&INSTRUCTION_POINTER_SET_IF_ZERO,
      &&INSTRUCTION_POINTER_SET_IF_NOT_ZERO,
      &&DATA_TRANSFER,
  };
  setupInstructionAddresses(begin, end, jumpTable);

  int64_t datas[30000] = {0};
  int64_t* data = &datas[0];
  int64_t data_dereferenced = *data;
  Instruction* instruction = const_cast<Instruction*>(begin);

  data += instruction->move;
  data_dereferenced = *data;

  goto*(instruction->jump);

NEXT: {
  instruction++;

  data += instruction->move;

  goto*(instruction->jump);
}

DATA_TRANSFER: {
  data_dereferenced = *data;
  const Instruction* last = instruction + instruction->value;
  while (instruction < last) {
    instruction++;
    *(data + instruction->move) += (data_dereferenced & 255) * instruction->value;
  }

  *data = 0;
  data_dereferenced = 0;

  goto NEXT;
}

INSTRUCTION_POINTER_SET_IF_NOT_ZERO: {
  data_dereferenced = *data;
  if ((data_dereferenced & 255) != 0) {
    instruction = instruction->next;

    data += instruction->move;
    data_dereferenced = *data;

    goto*(instruction->jump);
  }

  goto NEXT;
}

DATA_ADD: {
  *data += instruction->value;

  goto NEXT;
}

INSTRUCTION_POINTER_SET_IF_ZERO: {
  data_dereferenced = *data;
  if ((data_dereferenced & 255) == 0) {
    instruction = instruction->next;

    data += instruction->move;
    data_dereferenced = *data;

    goto*(instruction->jump);
  }

  goto NEXT;
}

DATA_PRINT: {
  output(*data, instruction->value);

  goto NEXT;
}

DATA_SET_FROM_INPUT: {
  input(data, instruction->value);

  goto NEXT;
}

DONE:
};

}  // namespace brainfuck
