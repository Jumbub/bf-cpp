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
  std::vector<std::pair<int64_t, char>> input;

  bool operator==(const LoopInput& rhs) const {
    return std::equal(this->input.begin(), this->input.end(), rhs.input.begin(), rhs.input.end());
  }
};

struct LoopOutput {
  std::map<int64_t, char> output;
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
  const auto read = [&](const int64_t offset) -> auto& {
    auto& value = *(data + offset);
    auto& wipHash = wipHashes.top();
    if (std::all_of(wipHash.input.cbegin(), wipHash.input.cend(), [&](std::pair<int64_t, char> cell) {
          return cell.first != (wipHash.moved + offset);
        })) {
      wipHashes.top().input.push_back({wipHash.moved + offset, value});
    }
    return value;
  };
  const auto print = [&](const char value) {
    std::cout << value;
    wipHashes.top().print.push_back(value);
  };
  const auto move = [&](const int64_t offset) {
    data += offset;
    wipHashes.top().moved += offset;
  };
  const auto add = [&](const int64_t offset, const char increment) {
    auto& value = read(offset);
    value += increment;
    wipHashes.top().output[wipHashes.top().moved + offset] = value;
  };
  const auto reset = [&]() {
    auto& value = read(0);  // todo: this really shouldnt be necessary surely, because it's unconditional
    value = 0;
    wipHashes.top().output[wipHashes.top().moved] = value;
  };
  //--------------------------------------------------------------------------------------

  move(instruction->move);
  goto*(instruction->jump);

NEXT: {
  instruction++;
  move(instruction->move);

  goto*(instruction->jump);
}

DATA_ADD: {
  add(0, static_cast<char>(instruction->value));

  goto NEXT;
}

DATA_TRANSFER: {
  const auto multiplier = read(0);
  const auto last = instruction->next;
  while (instruction < last) {
    instruction++;
    add(instruction->move, static_cast<char>(multiplier * instruction->value));
  }
  reset();

  goto NEXT;
}

INSTRUCTION_POINTER_SET_IF_NOT_ZERO: {
  const auto while_not_zero = instruction->next == instruction;
  while (while_not_zero && read(0) != 0) {
    move(instruction->move);
  }

  if (read(0) != 0) {
    instruction = instruction->next;
    move(instruction->move);

    goto*(instruction->jump);
  }

  goto NEXT;
}

INSTRUCTION_POINTER_SET_IF_ZERO: {
  if (read(0) == 0) {
    instruction = instruction->next;
    move(instruction->move);

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
