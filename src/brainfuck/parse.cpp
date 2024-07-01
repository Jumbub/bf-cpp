#include "parse.h"

#include <algorithm>
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
    current = std::next(current);
  }
  return loops.empty();  // check for brace mismatch
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
        break;
      case '-':
        instr.emplace_back(DATA_ADD, -consecutiveAccumulator(source_iterator, source.cend()));
        break;
      case '>':
        instr.emplace_back(DATA_POINTER_ADD, consecutiveAccumulator(source_iterator, source.cend()));
        break;
      case '<':
        instr.emplace_back(DATA_POINTER_ADD, -consecutiveAccumulator(source_iterator, source.cend()));
        break;
      case '.':
        instr.emplace_back(DATA_PRINT, consecutiveAccumulator(source_iterator, source.cend()));
        break;
      case ',':
        instr.emplace_back(DATA_SET_FROM_INPUT, consecutiveAccumulator(source_iterator, source.cend()));
        break;
      case '[':
        instr.emplace_back(INSTRUCTION_POINTER_SET_IF_ZERO);
        source_iterator = std::next(source_iterator);
        break;
      case ']':

        auto i = instr.end();
        // throw std::runtime_error((int)i);
        // auto last = std::next(i);
        // auto second_last = std::next(i, 2);
        if (std::prev(i)->type == DATA_ADD && std::prev(i)->value == -1 &&
            std::prev(i, 2)->type == INSTRUCTION_POINTER_SET_IF_ZERO) {
          instr.pop_back();
          instr.pop_back();
          instr.emplace_back(DATA_SET, 0);
        } else {
          instr.emplace_back(INSTRUCTION_POINTER_SET_IF_NOT_ZERO);
        }

        source_iterator = std::next(source_iterator);
        break;
    }
  }

  instr.emplace_back(DONE);

  if (!applyInstructionPointerOffsets(instr.begin(), instr.end())) {
    return std::nullopt;
  }

  return instr;
};

}  // namespace brainfuck
