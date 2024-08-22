#include "execute.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <stack>
#include <unordered_map>
#include <vector>

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

struct LoopInput {
  std::vector<std::pair<size_t, char>> input;

  bool operator==(const LoopInput& rhs) const {
    return std::equal(this->input.begin(), this->input.end(), rhs.input.begin(), rhs.input.end());
  }
};

struct LoopOutput {
  std::map<size_t, char> output;
  std::vector<char> print;
  int64_t moved;

  bool operator==(const LoopOutput& rhs) const {
    return this->moved == rhs.moved &&
           std::equal(this->print.begin(), this->print.end(), rhs.print.begin(), rhs.print.end()) &&
           std::equal(this->output.begin(), this->output.end(), rhs.output.begin(), rhs.output.end());
  }
};

struct Hash : public LoopInput, public LoopOutput {};

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

  std::stack<Hash> wipHashes;
  wipHashes.push({});

  //--------------------------------------------------------------------------------------
  const auto read = [&](int64_t offset) {
    const auto value = *(data + offset);
    return value;
  };
  const auto print = [&](char value) {
    std::cout << value;
    wipHashes.top().print.push_back(value);
  };
  //--------------------------------------------------------------------------------------

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
  const auto value = read(0);
  for (int i = 0; i < instruction->value; i++) {
    print(value);
  }

  goto NEXT;
}

DATA_SET_FROM_INPUT:
  throw std::runtime_error("unsupported");

DONE:
};

}  // namespace brainfuck

template <>
struct std::hash<brainfuck::LoopInput> {
  std::size_t operator()(const brainfuck::LoopInput& s) const noexcept { return s.input.size(); }
};
