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
using InitialValues = std::map<Data*, Data>;
using FinalValues = std::map<Data*, Data>;
using Outputs = std::vector<char>;
using Inputs = std::vector<char>;

struct Block {
  Hash start;
  InitialValues initial_value;
  FinalValues mutated_value;
  Outputs outputs;
  Inputs inputs;
  Data* final_data_pointer;
};

Hash makeHash(Instruction* instruction) {
  return reinterpret_cast<Hash>(instruction);
}

//

void observed(InitialValues& initial_values, Data* data) {
  if (!initial_values.contains(data)) {
    initial_values[data] = *data;
  }
}

Data observe(InitialValues& initial_values, Data* data) {
  observed(initial_values, data);

  return *data;
}

//

void mutated(FinalValues& final_values, Data* data, const Data value) {
  final_values[data] = value;
}

void mutate(FinalValues& final_values, Data* data, const Data value) {
  mutated(final_values, data, value);

  *data = value;
}

//

void printed(Outputs& outputs, const char value) {
  outputs.emplace_back(value);
}

void print(Outputs& outputs, const Data* data) {
  const char value = static_cast<char>(*data & 255);
  printed(outputs, value);
  std::cout << value;
}
//

void inputed(Inputs& inputs, const char input) {
  inputs.emplace_back(input);
}

char input(Inputs& inputs) {
  char input;
  std::cin >> std::noskipws >> input;
  inputed(inputs, input);
  return input;
}

//

void move(Data*& data, const Value value) {
  data += value;
}

//

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

  InitialValues initial_values;
  FinalValues final_values;
  Outputs outputs;
  Inputs inputs;

  const auto next = [](Instruction*& instruction, Instruction* next) { instruction = next; };

  move(data, instruction->move);
  goto*(instruction->jump);

NEXT: {
  next(instruction, instruction + 1);
  move(data, instruction->move);

  goto*(instruction->jump);
}

DATA_ADD: {
  mutate(final_values, data, observe(initial_values, data) + instruction->value);

  goto NEXT;
}

DATA_TRANSFER: {
  const auto multiplier = (observe(initial_values, data) & 255);
  const auto last = instruction->next;
  while (instruction < last) {
    next(instruction, instruction + 1);
    mutate(
        final_values, data + instruction->move,
        observe(initial_values, data + instruction->move) + multiplier * instruction->value);
  }
  mutate(final_values, data, 0);

  goto NEXT;
}

INSTRUCTION_POINTER_SET_IF_NOT_ZERO: {
  const auto while_not_zero = instruction->next == instruction;
  while (while_not_zero && (observe(initial_values, data) & 255) != 0) {
    move(data, instruction->move);
  }

  if ((observe(initial_values, data) & 255) != 0) {
    next(instruction, instruction->next);
    move(data, instruction->move);

    goto*(instruction->jump);
  }

  goto NEXT;
}

INSTRUCTION_POINTER_SET_IF_ZERO: {
  if ((observe(initial_values, data) & 255) == 0) {
    next(instruction, instruction->next);
    move(data, instruction->move);

    goto*(instruction->jump);
  }

  goto NEXT;
}

DATA_PRINT: {
  for (int i = 0; i < instruction->value; i++) {
    print(outputs, data);
  }

  goto NEXT;
}

DATA_SET_FROM_INPUT: {
  for (int i = 0; i < instruction->value; i++) {
    const Data value = input(inputs);
    if (!std::cin.eof()) {
      mutate(final_values, data, value);
    }
  }

  goto NEXT;
}

DONE:
};

}  // namespace brainfuck
