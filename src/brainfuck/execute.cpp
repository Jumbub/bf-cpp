#include "execute.h"

#include <iostream>

namespace brainfuck {

void output(const int64_t output, const Value times) {
  for (int i = 0; i < times; i++) {
    std::cout << static_cast<char>(output % 256);
  }
}

void input(int64_t* character, const Value times) {
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
    if (current->type == DATA_TRANSFER) {
      current->next = current + current->value;
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
      nullptr,
      &&DONE,
      &&DATA_ADD,
      &&DATA_SET_FROM_INPUT,
      &&DATA_PRINT,
      nullptr,
      &&INSTRUCTION_POINTER_SET_IF_ZERO,
      &&INSTRUCTION_POINTER_SET_IF_NOT_ZERO,
      &&DATA_TRANSFER,
      nullptr,
      &&DATA_SET,
      &&DATA_SCAN,
      &&DATA_RESET_MANY,
  };
  setupInstructionAddresses(begin, end, jumpTable);

  int64_t datas[30000] = {0};
  int64_t* data = &datas[0];
  Instruction* instruction = const_cast<Instruction*>(begin);

  data += instruction->move;
  goto*(instruction->jump);

NEXT: {
  instruction++;
  data += instruction->move;

  goto*(instruction->jump);
}

DATA_ADD: {
  *data += instruction->value;

  goto NEXT;
}

DATA_TRANSFER: {
  const auto multiplier = (*data & 255);
  const auto last = instruction->next;
  while (instruction < last) {
    instruction++;
    *(data + instruction->move) += multiplier * instruction->value;
  }
  *data = 0;

  goto NEXT;
}

INSTRUCTION_POINTER_SET_IF_NOT_ZERO: {
  const auto while_not_zero = instruction->next == instruction;
  while (while_not_zero && (*data & 255) != 0) {
    data += instruction->move;
  }

  if ((*data & 255) != 0) {
    instruction = instruction->next;
    data += instruction->move;

    goto*(instruction->jump);
  }

  goto NEXT;
}

DATA_SET: {
  *data = instruction->value;

  goto NEXT;
}

DATA_SCAN: {
  while (*data != 0) {
    data += instruction->value;
  }

  goto NEXT;
}

DATA_RESET_MANY: {
  *data = 0;

  const auto last_data_pointer = data + instruction->value;
  while (data < last_data_pointer) {
    data += 1;
    *data = 0;
  }

  goto NEXT;
}

INSTRUCTION_POINTER_SET_IF_ZERO: {
  if ((*data & 255) == 0) {
    instruction = instruction->next;
    data += instruction->move;

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
