#include "parse.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <stack>
#include <stdexcept>

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

[[nodiscard]] int64_t consecutiveAccumulator(Source::const_iterator& begin, const Source::const_iterator end) noexcept {
  const char check = *begin;
  const auto miss = std::find_if(begin, end, [check](char current) -> bool { return current != check; });
  const auto distance = std::distance(begin, miss);
  begin = miss;
  return distance;
}

[[nodiscard]] bool applyInstructionPointerOffsets(
    const Instructions::iterator begin,
    const Instructions::iterator end) noexcept {
  std::stack<Instructions::iterator> loops;
  Instructions::iterator current = begin;
  while (current < end) {
    if (current->type == INSTRUCTION_POINTER_SET_IF_ZERO) {
      loops.push(current);
    } else if (current->type == INSTRUCTION_POINTER_SET_IF_NOT_ZERO) {
      if (loops.empty()) {  // check for brace mismatch
        return false;
      }
      const auto open = loops.top();
      loops.pop();

      open->value = std::distance(begin, current) + 1;
      current->value = std::distance(begin, open) + 1;
    }
    std::advance(current, 1);
  }
  return loops.empty();  // check for brace mismatch
}

void tryOptimiseLoop(Instructions& instr) {
  int64_t offset = 0;
  std::map<Move, Value> transfers;

  auto current = instr.rbegin();
  if (current->type != INSTRUCTION_POINTER_SET_IF_NOT_ZERO) {
    throw std::runtime_error("bad1");
  }
  offset -= current->move;  // end loop
  std::advance(current, 1);

  while (current != instr.rend()) {
    if (current->type == DATA_POINTER_ADD) {
      throw std::runtime_error("bad2");
    }
    if (current->type == DATA_ADD) {
      offset -= current->move;
      transfers[offset] += current->value;
    } else if (current->type == INSTRUCTION_POINTER_SET_IF_ZERO) {
      break;  // we're at the start of the loop
    } else {
      return;  // unoptimisable loop
    }
    std::advance(current, 1);
  }

  const auto startMove = current->move;

  if (offset != 0 || !transfers.contains(0) || transfers[0] != -1) {
    return;
  }

  const auto dist = std::distance(instr.rbegin(), current);
  for (int i = 0; i >= -dist; i--) {
    instr.pop_back();
  }

  transfers.erase(0);
  instr.emplace_back(DATA_TRANSFER, transfers.size(), startMove);
  for (const auto& [offset, value] : transfers) {
    instr.emplace_back(DATA_TRANSFER_META, value, offset);
  }
}

void tryOptimiseInstructions(Instructions& instr) {
  if (instr.size() < 2) {
    return;
  }

  const auto reverse = instr.end();
  auto last = std::prev(reverse, 1);
  auto secondLast = std::prev(reverse, 2);

  while (last->type == DATA_TRANSFER || last->type == DATA_TRANSFER_META) {
    return;
    last = std::prev(last);
    secondLast = std::prev(secondLast);
  }

  if (secondLast->type == DATA_POINTER_ADD) {
    secondLast->move = secondLast->value;
    secondLast->type = last->type;
    secondLast->value = last->value;
    instr.pop_back();
  }
}

void tryOptimiseDataPointerAdd(Instructions& instr) {
  if (instr.size() < 2) {
    return;
  }

  const auto reverse = instr.end();
  auto last = std::prev(reverse, 1);
  auto secondLast = std::prev(reverse, 2);

  if (secondLast->type == DATA_POINTER_ADD) {
    secondLast->value += last->value;
    instr.pop_back();
  }
}

[[nodiscard]] std::optional<std::vector<Instruction>> parse(const std::vector<char> plaintext) {
  const Source source = removeNoopCodes(plaintext);

  std::vector<Instruction> instr;
  instr.reserve(source.size());

  Source::const_iterator source_iterator = source.cbegin();
  while (source_iterator != source.cend()) {
    switch (*source_iterator) {
      case '+':
        instr.emplace_back(DATA_ADD, consecutiveAccumulator(source_iterator, source.cend()));
        tryOptimiseInstructions(instr);
        break;
      case '-':
        instr.emplace_back(DATA_ADD, -consecutiveAccumulator(source_iterator, source.cend()));
        tryOptimiseInstructions(instr);
        break;
      case '>':
        instr.emplace_back(DATA_POINTER_ADD, consecutiveAccumulator(source_iterator, source.cend()));
        tryOptimiseDataPointerAdd(instr);
        break;
      case '<':
        instr.emplace_back(DATA_POINTER_ADD, -consecutiveAccumulator(source_iterator, source.cend()));
        tryOptimiseDataPointerAdd(instr);
        break;
      case '.':
        instr.emplace_back(DATA_PRINT, consecutiveAccumulator(source_iterator, source.cend()));
        tryOptimiseInstructions(instr);
        break;
      case ',':
        instr.emplace_back(DATA_SET_FROM_INPUT, consecutiveAccumulator(source_iterator, source.cend()));
        tryOptimiseInstructions(instr);
        break;
      case '[':
        instr.emplace_back(INSTRUCTION_POINTER_SET_IF_ZERO);
        tryOptimiseInstructions(instr);
        std::advance(source_iterator, 1);
        break;
      case ']':
        instr.emplace_back(INSTRUCTION_POINTER_SET_IF_NOT_ZERO);
        tryOptimiseInstructions(instr);
        // tryOptimiseLoop(instr);
        std::advance(source_iterator, 1);
        break;
    }
  }

  instr.emplace_back(DONE);
  tryOptimiseInstructions(instr);

  if (!applyInstructionPointerOffsets(instr.begin(), instr.end())) {
    return std::nullopt;
  }

  return instr;
};

}  // namespace brainfuck
