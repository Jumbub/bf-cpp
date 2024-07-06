#include "execute.h"

#include <iostream>
#include <map>
#include <stack>
#include <vector>

namespace brainfuck {

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

//

using Data = int64_t;
using Hash = Instruction*;

struct Block {
  Hash start;
  std::map<int16_t, char> initial_value;
  std::map<int16_t, char> mutated_value;
  std::vector<char> output;
  Data* final_data_pointer;
};

//
Hash makeHash(Instruction* instruction) {
  return reinterpret_cast<Hash>(instruction);
}

Data observed(const Data* data) {
  return *data;
  // auto initial_values = in_progress_blocks.top().initial_values;
  // if (!initial_values.contains(data)) {
  //   initial_values[data] = static_cast<char>(*data);
  // }
}

void mutated(Data* data, const Data value) {
  *data = value;
  // in_progress_blocks.top().initial_values[data] = static_cast<char>(*data);
}

void moved(Data*& data, const Value value) {
  data += value;
  // in_progress_blocks.top().final_data_pointer = data;
}

void printed(const Data* data) {
  std::cout << static_cast<char>(*data & 255);
  // in_progress_blocks.top().output.push_back(static_cast<char>(*data));
}

Data inputed() {
  char input;
  std::cin >> std::noskipws >> input;
  return input;
  // in_progress_blocks.top().input.push_back(static_cast<char>(*data));
}

void next(Instruction*& instruction) {
  instruction++;
  // in_progress_blocks.top().input.push_back(static_cast<char>(*data));
}

void next(Instruction*& instruction, Instruction* next) {
  instruction = next;
  // in_progress_blocks.top().input.push_back(static_cast<char>(*data));
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

  Data datas[30000] = {0};
  Data* data = &datas[0];
  Instruction* instruction = const_cast<Instruction*>(begin);

  std::stack<Block> in_progress_blocks;

  moved(data, instruction->move);
  goto*(instruction->jump);

NEXT: {
  next(instruction);
  moved(data, instruction->move);

  goto*(instruction->jump);
}

DATA_ADD: {
  mutated(data, observed(data) + instruction->value);

  goto NEXT;
}

DATA_TRANSFER: {
  const auto multiplier = (observed(data) & 255);
  const auto last = instruction->next;
  while (instruction < last) {
    next(instruction);
    mutated(data + instruction->move, observed(data + instruction->move) + multiplier * instruction->value);
  }
  mutated(data, 0);

  goto NEXT;
}

INSTRUCTION_POINTER_SET_IF_NOT_ZERO: {
  const auto while_not_zero = instruction->next == instruction;
  while (while_not_zero && (observed(data) & 255) != 0) {
    moved(data, instruction->move);
  }

  if ((observed(data) & 255) != 0) {
    next(instruction, instruction->next);
    moved(data, instruction->move);

    goto*(instruction->jump);
  }

  goto NEXT;
}

INSTRUCTION_POINTER_SET_IF_ZERO: {
  if ((observed(data) & 255) == 0) {
    next(instruction, instruction->next);
    moved(data, instruction->move);

    goto*(instruction->jump);
  }

  goto NEXT;
}

DATA_PRINT: {
  for (int i = 0; i < instruction->value; i++) {
    printed(data);
  }

  goto NEXT;
}

DATA_SET_FROM_INPUT: {
  for (int i = 0; i < instruction->value; i++) {
    const Data input = inputed();
    if (!std::cin.eof()) {
      mutated(data, input);
    }
  }

  goto NEXT;
}

DONE:
};

}  // namespace brainfuck
