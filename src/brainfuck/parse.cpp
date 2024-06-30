#include "parse.h"

#include <algorithm>
#include <stack>
#include <stdexcept>

namespace brainfuck {

using Source = std::vector<char>;

[[nodiscard]] int64_t consecutiveAccumulator(Source::const_iterator& begin, const Source::const_iterator end) noexcept {
  const char check = *begin;
  const auto miss = std::find_if(begin, end, [check](char current) -> bool { return current != check; });
  const auto distance = std::distance(begin, miss);
  begin = miss;
  return distance;
}

[[nodiscard]] inline const Source removeNoopCodes(const Source input) noexcept {
  Source output;
  output.reserve(input.size());
  std::copy_if(input.begin(), input.end(), std::back_inserter(output), [](const char output) {
    return output == '+' || output == '-' || output == '>' || output == '<' || output == '.' || output == ',' ||
           output == '[' || output == ']';
  });

  return output;
}

template <Type T>
bool isType(const Instruction& value) {
  return value.type == T;
}

using Instructions = std::vector<Instruction>;
bool applyInstructionPointerOffsets(Instructions& instructions) noexcept {
  std::stack<size_t> loops;

  for (size_t i = 0; i < instructions.size(); i++) {
    if (instructions[i].type == INSTRUCTION_POINTER_SET_IF_ZERO) {
      loops.push(i);
    } else if (instructions[i].type == INSTRUCTION_POINTER_SET_IF_NOT_ZERO) {
      if (loops.empty()) {
        return false;
      }

      const auto startI = loops.top();
      loops.pop();

      instructions[startI].value = static_cast<Value>(i + 1);
      instructions[i].value = static_cast<Value>(startI + 1);
    }
  }

  return loops.empty();
}

std::optional<std::vector<Instruction>> parse(const std::vector<char> plaintext) {
  const Source source = removeNoopCodes(plaintext);

  std::vector<Instruction> instr;
  instr.reserve(source.size());

  Source::const_iterator source_iterator = source.cbegin();
  while (source_iterator != source.cend()) {
    if (*source_iterator == '+') {
      instr.emplace_back(DATA_ADD, consecutiveAccumulator(source_iterator, source.cend()));
    } else if (*source_iterator == '-') {
      instr.emplace_back(DATA_ADD, -consecutiveAccumulator(source_iterator, source.cend()));
    } else if (*source_iterator == '>') {
      instr.emplace_back(DATA_POINTER_ADD, 0, consecutiveAccumulator(source_iterator, source.cend()));
    } else if (*source_iterator == '<') {
      instr.emplace_back(DATA_POINTER_ADD, 0, -consecutiveAccumulator(source_iterator, source.cend()));
    } else if (*source_iterator == '.') {
      instr.emplace_back(DATA_PRINT, consecutiveAccumulator(source_iterator, source.cend()));
    } else if (*source_iterator == ',') {
      instr.emplace_back(DATA_SET_FROM_INPUT, consecutiveAccumulator(source_iterator, source.cend()));
    } else if (*source_iterator == '[') {
      instr.emplace_back(INSTRUCTION_POINTER_SET_IF_ZERO);
      source_iterator = std::next(source_iterator);
    } else if (*source_iterator == ']') {
      instr.emplace_back(INSTRUCTION_POINTER_SET_IF_NOT_ZERO);
      source_iterator = std::next(source_iterator);
    }
  }

  instr.emplace_back(DONE);

  if (!applyInstructionPointerOffsets(instr)) {
    return std::nullopt;
  }

  return instr;
};

}  // namespace brainfuck
