#include "parse.h"

#include <stack>

namespace brainfuck {

std::expected<ByteCode, Error> parse(const Code code) {
  ByteCode instr;
  instr.reserve(code.size() + 1);
  instr.emplace_back(NOOP, 0);

  std::stack<int> starting_brace_positions;

  // int instr_i = 1;  // operates with +1 offset to simplify optimisation logic
  for (int code_i = 0; code_i < code.size(); code_i++) {
    if (code[code_i] == '+') {
      if (instr.back().type == MUTATE_DATA) {
        instr.back().value += 1;
      } else {
        instr.emplace_back(MUTATE_DATA, 1);
      }
    } else if (code[code_i] == '-') {
      if (instr.back().type == MUTATE_DATA) {
        instr.back().value -= 1;
      } else {
        instr.emplace_back(MUTATE_DATA, -1);
      }
    } else if (code[code_i] == '>') {
      if (instr.back().type == MUTATE_DATA_POINTER) {
        instr.back().value += 1;
      } else {
        instr.emplace_back(MUTATE_DATA_POINTER, 1);
      }
    } else if (code[code_i] == '<') {
      if (instr.back().type == MUTATE_DATA_POINTER) {
        instr.back().value -= 1;
      } else {
        instr.emplace_back(MUTATE_DATA_POINTER, -1);
      }
    } else if (code[code_i] == '[') {
      instr.emplace_back(MUTATE_INSTRUCTION_POINTER_IF_ZERO, 0);
      const auto currentIndex = instr.size() - 1;
      starting_brace_positions.push(currentIndex);
    } else if (code[code_i] == ']') {
      if (starting_brace_positions.empty()) {
        return std::unexpected(Error::UNMATCHED_BRACE);
      }
      const int starting_brace_position = starting_brace_positions.top();
      starting_brace_positions.pop();

      instr.emplace_back(MUTATE_INSTRUCTION_POINTER_IF_NOT_ZERO, starting_brace_position + 1);

      const auto currentIndex = instr.size() - 1;
      instr[starting_brace_position].value = currentIndex + 1;
    } else if (code[code_i] == '.') {
      instr.emplace_back(WRITE, 0);
    } else if (code[code_i] == ',') {
      instr.emplace_back(READ, 0);
    }
  }

  if (!starting_brace_positions.empty()) {
    return std::unexpected(Error::UNMATCHED_BRACE);
  }

  return instr;
};

Instruction::Instruction(Type type, int value) : type(type), value(value){};

}  // namespace brainfuck
