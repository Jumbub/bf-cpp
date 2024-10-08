#include "parse.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <stack>
#include <stdexcept>
#include <tuple>

namespace brainfuck {

using Source = std::vector<char>;
using Instructions = std::vector<Instruction>;

[[nodiscard]] inline Source removeNoopCodes(const Source input) noexcept {
  Source output;
  output.reserve(input.size());
  std::copy_if(input.begin(), input.end(), std::back_inserter(output), [](const char output) {
    return output == '+' || output == '-' || output == '>' || output == '<' || output == '.' || output == ',' ||
           output == '[' || output == ']';
  });
  return output;
}

[[nodiscard]] std::tuple<Source::const_iterator, Source::difference_type> consecutiveAccumulator(
    const Source::const_iterator& begin,
    const Source::const_iterator end) noexcept {
  const char check = *begin;
  const auto miss = std::find_if(begin, end, [check](char current) -> bool { return current != check; });
  const auto distance = std::distance(begin, miss);
  return {miss, distance};
}

[[nodiscard]] bool applyInstructionPointerOffsets(
    const Instructions::iterator begin,
    const Instructions::iterator end) noexcept {
  std::map<ssize_t, Instructions::iterator> loops;
  Instructions::iterator current = begin;
  while (current < end) {
    if (current->type == INSTRUCTION_POINTER_SET_IF_ZERO) {
      loops[current->value] = current;
    } else if (current->type == INSTRUCTION_POINTER_SET_IF_NOT_ZERO) {
      if (loops.contains(current->value)) {  // check for brace mismatch
        const auto open = loops[current->value];
        open->value = std::distance(begin, current) + 1;
        current->value = std::distance(begin, open) + 1;
      } else {
        current->value = std::distance(begin, current);
      }
    }
    std::advance(current, 1);
  }
  return true;
}

[[nodiscard]] Instructions optimize(Instructions::const_iterator current, const Instructions::const_iterator end) {
  Instructions instr_out;
  instr_out.reserve(static_cast<size_t>(std::distance(current, end)));

  while (current < end) {
    if (current->type == INSTRUCTION_POINTER_SET_IF_NOT_ZERO && instr_out.back().value == current->value &&
        instr_out.back().move == current->move) {
      instr_out.back().type = INSTRUCTION_POINTER_SET_IF_NOT_ZERO;
    } else {
      instr_out.emplace_back(current->type, current->value, current->move);
    }
    std::advance(current, 1);
  }

  return instr_out;
}

[[nodiscard]] Instructions squashPointerAddInstructions(
    Instructions::const_iterator current,
    const Instructions::const_iterator end) {
  Instructions instr_out;
  instr_out.reserve(static_cast<size_t>(std::distance(current, end) / 2));

  while (current < end) {
    Move move = current->move;
    while (current->type == DATA_POINTER_ADD) {
      if (move != 0) {
        throw std::runtime_error("expected move to be 0");
      }
      move = current->value;
      std::advance(current, 1);
    }
    if (current == end) {
      throw std::runtime_error("expected DONE to be final instruction");
    }
    instr_out.emplace_back(current->type, current->value, move);
    std::advance(current, 1);
  }

  return instr_out;
}

void tryOptimiseLoop(Instructions& instr) {
  int64_t offset = 0;
  std::map<Move, Value> transfers;

  auto current = instr.rbegin();
  if (current->type != INSTRUCTION_POINTER_SET_IF_NOT_ZERO) {
    throw std::runtime_error("expected INSTRUCTION_POINTER_SET_IF_NOT_ZERO when running tryOptimiseLoop");
  }
  std::advance(current, 1);

  while (current != instr.rend()) {
    if (current->type == DATA_POINTER_ADD) {
      offset -= current->value;  // going backwards so invert the movement
    } else if (current->type == DATA_ADD) {
      transfers[offset] += current->value;
    } else if (current->type == INSTRUCTION_POINTER_SET_IF_ZERO) {
      break;  // we're at the start of the loop
    } else {
      return;  // unoptimisable loop
    }
    std::advance(current, 1);
  }

  if (offset != 0 || !transfers.contains(0) || transfers[0] != -1) {
    return;
  }

  const auto dist = std::distance(instr.rbegin(), current);
  for (int i = 0; i >= -dist; i--) {
    instr.pop_back();
  }

  transfers.erase(0);
  instr.emplace_back(DATA_TRANSFER, transfers.size());
  for (const auto& [offset, value] : transfers) {
    instr.emplace_back(DATA_TRANSFER_META, value, offset);
  }

  return;
}

void addValue(Instructions& instr, const Type type, const Value value) {
  if (instr.size() > 0 && instr.back().type == type) {
    instr.back().value += value;
    if (instr.back().value == 0) {
      instr.pop_back();
    }
  } else {
    instr.emplace_back(type, value);
  }
}

struct BraceIds {
  size_t next_brace_pair_id = 0;
  std::stack<size_t> brace_pair_ids;

  auto push() {
    brace_pair_ids.push(next_brace_pair_id);
    next_brace_pair_id += 1;
    return brace_pair_ids.top();
  };

  auto pop() {
    const auto value = brace_pair_ids.top();
    brace_pair_ids.pop();
    return value;
  };
};

[[nodiscard]] std::optional<std::vector<Instruction>> parse(const std::vector<char> plaintext) {
  const Source source = removeNoopCodes(plaintext);

  std::vector<Instruction> instr;
  instr.reserve(source.size());

  BraceIds braceIds;

  Source::const_iterator source_iterator = source.cbegin();
  while (source_iterator != source.cend()) {
    switch (*source_iterator) {
      case '+': {
        const auto [next_iterator, value] = consecutiveAccumulator(source_iterator, source.end());
        addValue(instr, DATA_ADD, value);
        source_iterator = next_iterator;
      } break;
      case '-': {
        const auto [next_iterator, value] = consecutiveAccumulator(source_iterator, source.end());
        addValue(instr, DATA_ADD, -value);
        source_iterator = next_iterator;
      } break;
      case '>': {
        const auto [next_iterator, value] = consecutiveAccumulator(source_iterator, source.end());
        addValue(instr, DATA_POINTER_ADD, value);
        source_iterator = next_iterator;
      } break;
      case '<': {
        const auto [next_iterator, value] = consecutiveAccumulator(source_iterator, source.end());
        addValue(instr, DATA_POINTER_ADD, -value);
        source_iterator = next_iterator;
      } break;
      case '.': {
        const auto [next_iterator, value] = consecutiveAccumulator(source_iterator, source.end());
        addValue(instr, DATA_PRINT, value);
        source_iterator = next_iterator;
      } break;
      case ',': {
        const auto [next_iterator, value] = consecutiveAccumulator(source_iterator, source.end());
        addValue(instr, DATA_SET_FROM_INPUT, value);
        source_iterator = next_iterator;
      } break;
      case '[':
        instr.emplace_back(INSTRUCTION_POINTER_SET_IF_ZERO, braceIds.push());
        std::advance(source_iterator, 1);
        break;
      case ']':
        instr.emplace_back(INSTRUCTION_POINTER_SET_IF_NOT_ZERO, braceIds.pop());
        tryOptimiseLoop(instr);
        std::advance(source_iterator, 1);
        break;
    }
  }

  if (!braceIds.brace_pair_ids.empty()) {
    throw std::runtime_error("nope!");
  }

  instr.emplace_back(DONE);

  instr = squashPointerAddInstructions(instr.cbegin(), instr.cend());
  instr = optimize(instr.cbegin(), instr.cend());

  if (!applyInstructionPointerOffsets(instr.begin(), instr.end())) {
    return std::nullopt;
  }

  return instr;
};

}  // namespace brainfuck
