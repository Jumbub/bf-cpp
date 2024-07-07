#include "execute.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
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
  InitialValues initial_value;
  FinalValues final_values;
  Outputs outputs;
  Inputs inputs;
  Data* final_data_pointer;
};

Hash makeHash(Instruction* instruction) {
  return reinterpret_cast<Hash>(instruction);
}

//

void observed(InitialValues& initial_values, Data* data, const Data value) {
  if (!initial_values.contains(data)) {
    initial_values[data] = value;
  }
}

Data observe(InitialValues& initial_values, Data* data) {
  observed(initial_values, data, *data);

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

void next(Instruction*& instruction, Instruction* next) {
  instruction = next;
}

using SolvedBlocks = std::map<Hash, std::vector<Block>>;

bool missing_solution(const std::vector<Block>& haystack, const Block& needle) {
  return !std::any_of(haystack.cbegin(), haystack.cend(), [&](const Block& block) {
    return block.final_data_pointer == needle.final_data_pointer && block.initial_value == needle.initial_value &&
           block.final_values == needle.final_values && block.inputs == needle.inputs &&
           block.outputs == needle.outputs;
  });
}

int ii = 0;

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

  std::stack<Instruction*> first_instructions;
  SolvedBlocks solved_blocks;
  std::stack<Block> in_progress_blocks;
  InitialValues initial_values;
  FinalValues final_values;
  Outputs outputs;
  Inputs inputs;

  const auto start = [&]() {
    if (solved_blocks.contains(instruction)) {
      // std::cout << "hello?" << std::endl;
      const auto matching_blocks = solved_blocks[instruction];
      const auto solved = std::find_if(matching_blocks.cbegin(), matching_blocks.cend(), [&](const Block& block) {
        return std::all_of(
            block.initial_value.cbegin(), block.initial_value.cend(),
            [](const std::pair<Data*, Data>& pair) { return (*pair.first) == pair.second; });
      });
      if (solved != matching_blocks.cend() && solved->inputs.size() == 0) {
        for (const auto& [key, value] : solved->initial_value) {
          observed(initial_values, key, value);
        }
        for (const auto& [key, value] : solved->final_values) {
          mutated(initial_values, key, value);
        }
        for (const auto output : solved->outputs) {
          printed(outputs, output);
        }
        return;
      }
    }

    in_progress_blocks.emplace(Block{
        .initial_value = {},
        .final_values = {},
        .outputs = {},
        .inputs = {},
        .final_data_pointer = data,
    });

    first_instructions.push(instruction);
    initial_values.clear();
    final_values.clear();
    outputs.clear();
    inputs.clear();
  };

  const auto finish = [&]() {
    if (in_progress_blocks.empty()) {
      throw std::runtime_error("ohno");
    }

    const auto solved = Block{
        .initial_value = initial_values,
        .final_values = final_values,
        .outputs = outputs,
        .inputs = inputs,
        .final_data_pointer = data,
    };

    if (missing_solution(solved_blocks[first_instructions.top()], solved)) {
      solved_blocks[first_instructions.top()].push_back(solved);
    }

    in_progress_blocks.pop();
    first_instructions.pop();

    initial_values = in_progress_blocks.top().initial_value;
    final_values = in_progress_blocks.top().final_values;
    outputs = in_progress_blocks.top().outputs;
    inputs = in_progress_blocks.top().inputs;

    for (const auto& [key, value] : solved.initial_value) {
      observed(initial_values, key, value);
    }
    for (const auto& [key, value] : solved.final_values) {
      mutated(initial_values, key, value);
    }
    for (const auto output : solved.outputs) {
      printed(outputs, output);
    }
    for (const auto input : solved.inputs) {
      inputed(inputs, input);
    }
  };

  start();
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

  finish();
  goto NEXT;
}

INSTRUCTION_POINTER_SET_IF_ZERO: {
  start();

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
