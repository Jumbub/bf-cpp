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
  std::vector<std::pair<int64_t, char>> input = {};

  bool operator==(const LoopInput& rhs) const {
    return std::equal(this->input.begin(), this->input.end(), rhs.input.begin(), rhs.input.end());
  }
};

struct LoopOutput {
  std::map<int64_t, char> output = {};
  std::vector<char> print = {};
  int64_t moved = 0;

  bool operator==(const LoopOutput& rhs) const {
    return this->moved == rhs.moved &&
           std::equal(this->print.begin(), this->print.end(), rhs.print.begin(), rhs.print.end()) &&
           std::equal(this->output.begin(), this->output.end(), rhs.output.begin(), rhs.output.end());
  }
};

struct Hash : public LoopInput, public LoopOutput {};

}  // namespace brainfuck

template <>
struct std::hash<brainfuck::LoopInput> {
  std::size_t operator()(const brainfuck::LoopInput& s) const noexcept { return s.input.size(); }
};

void brainfuck::execute(const Instruction* begin, const Instruction* end) {
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

  //--------------------------------------------------------------------------------------
  std::stack<Hash> wipHashes;
  wipHashes.push({});
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
  const auto execute = [&](const LoopInput& query, const LoopOutput& solution) {
    auto& wipHash = wipHashes.top();

    for (const char character : solution.print) {
      std::cout << character << std::flush;  // TODO: remove flush after done debugging
      wipHash.print.push_back(character);
    }

    for (const auto& [relativeOffset, character] : query.input) {
      if (std::all_of(wipHash.input.cbegin(), wipHash.input.cend(), [&](std::pair<int64_t, char> cell) {
            return cell.first != wipHash.moved + relativeOffset;
          })) {
        wipHash.input.push_back({wipHash.moved + relativeOffset, character});
      }
    }

    for (const auto& [relativeOffset, character] : solution.output) {
      *(data + relativeOffset) = character;
      wipHash.output[wipHash.moved + relativeOffset] = character;
    }

    data += solution.moved;
    wipHash.moved += solution.moved;
  };

  std::unordered_map<const Instruction*, std::unordered_map<LoopInput, LoopOutput>> solves;
  const auto checkIfSolved = [&]() -> std::optional<std::pair<const LoopInput, const LoopOutput>> {
    std::cout << "hi" << std::endl;
    for (const auto& [in, out] : solves[instruction]) {
      const auto matches = std::all_of(in.input.cbegin(), in.input.cend(), [&](auto cell) {
        const auto& [relativeOffset, character] = cell;
        return *(data + relativeOffset) == character;
      });
      if (matches) {
        return std::pair{in, out};  // TODO: simplify to a std::find
      }
    }
    return std::nullopt;
  };
  const auto maybeExecuteLoopInCache = [&]() -> bool {
    const auto solution = checkIfSolved();
    if (solution.has_value()) {
      execute(solution.value().first, solution.value().second);
      return true;
    }
    return false;
  };
  const auto maybeStoreLoopCache = [&]() {
    const Hash solvedHash = wipHashes.top();

    if (solves[instruction->next].contains(solvedHash)) {
      throw std::runtime_error("cache was not used");
    }
    solves[instruction->next][solvedHash] = solvedHash;

    wipHashes.pop();
    auto& wipHash = wipHashes.top();

    for (const auto& [relativeOffset, character] : solvedHash.input) {
      if (std::all_of(wipHash.input.cbegin(), wipHash.input.cend(), [&](std::pair<int64_t, char> cell) {
            return cell.first != wipHash.moved + relativeOffset;
          })) {
        wipHash.input.push_back({wipHash.moved + relativeOffset, character});
      }
    }

    for (const auto& [relativeOffset, character] : solvedHash.output) {
      std::cout << wipHash.output.size() << std::endl;
      wipHash.output[wipHash.moved + relativeOffset] = character;
    }

    for (const auto character : solvedHash.print) {
      wipHash.print.push_back(character);
    }

    wipHash.moved += solvedHash.moved;
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

  maybeStoreLoopCache();

  goto NEXT;
}

INSTRUCTION_POINTER_SET_IF_ZERO: {
  const auto skip = read(0) == 0 || maybeExecuteLoopInCache();

  if (skip) {
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
