#include "execute.h"

#include <iostream>

namespace brainfuck {

void output(const char output, const Value times) {
  for (int i = 0; i < times; i++) {
    std::cout << output;
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

  char datas[30000] = {0};
  char* data = &datas[0];
  Instruction* instruction = const_cast<Instruction*>(begin);

  data += instruction->move;
  goto*(instruction->jump);

NEXT: {
  instruction++;
  data += instruction->move;

  goto*(instruction->jump);
}

DATA_ADD: {
  *data += static_cast<char>(instruction->value);

  goto NEXT;
}

DATA_TRANSFER: {
  const auto multiplier = *data;
  const auto last = instruction->next;
  while (instruction < last) {
    instruction++;
    *(data + instruction->move) += static_cast<char>(multiplier * instruction->value);
  }
  *data = 0;

  goto NEXT;
}

INSTRUCTION_POINTER_SET_IF_NOT_ZERO: {
  const auto while_not_zero = instruction->next == instruction;
  while (while_not_zero && *data != 0) {
    data += instruction->move;
  }

  if (*data != 0) {
    instruction = instruction->next;
    data += instruction->move;

    goto*(instruction->jump);
  }

  goto NEXT;
}

INSTRUCTION_POINTER_SET_IF_ZERO: {
  if (*data == 0) {
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

DATA_SET_FROM_INPUT:
  throw std::runtime_error("unsupported");

DONE:
};

}  // namespace brainfuck
