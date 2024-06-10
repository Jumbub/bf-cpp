#include "parse.h"

#include <stack>

namespace brainfuck {

std::expected<ByteCode, Error> parse(const std::vector<char> code) {
  ByteCode instr;
  instr.reserve(code.size() + 1);

  std::stack<int> starting_brace_positions;

  int instr_i = 1;  // operates with +1 offset to simplify optimisation logic
  for (int code_i = 0; code_i < code.size(); code_i++) {
    switch (code[code_i]) {
      case '+':
        if (instr[instr_i - 1].type == MUTATE_DATA) {
          instr[instr_i - 1].value += 1;
        } else {
          instr[instr_i].type = MUTATE_DATA;
          instr[instr_i].value = 1;
          instr_i++;
        }
        break;
      case '-':
        if (instr[instr_i - 1].type == MUTATE_DATA) {
          instr[instr_i - 1].value -= 1;
        } else {
          instr[instr_i].type = MUTATE_DATA;
          instr[instr_i].value = -1;
          instr_i++;
        }
        break;
      case '>':
        if (instr[instr_i - 1].type == MUTATE_DATA_POINTER) {
          instr[instr_i - 1].value += 1;
        } else {
          instr[instr_i].type = MUTATE_DATA_POINTER;
          instr[instr_i].value = 1;
          instr_i++;
        }
        break;
      case '<':
        if (instr[instr_i - 1].type == MUTATE_DATA_POINTER) {
          instr[instr_i - 1].value -= 1;
        } else {
          instr[instr_i].type = MUTATE_DATA_POINTER;
          instr[instr_i].value = -1;
          instr_i++;
        }
        break;
      case '.':
        instr[instr_i].type = WRITE;
        instr_i++;
        break;
      case ',':
        instr[instr_i].type = READ;
        instr_i++;
        break;
      case '[':
        instr[instr_i].type = MUTATE_INSTRUCTION_POINTER_IF_ZERO;
        starting_brace_positions.push(instr_i);
        instr_i++;
        break;
      case ']':
        if (starting_brace_positions.empty()) {
          return std::unexpected(Error::UNMATCHED_BRACE);
        }
        const int starting_brace_position = starting_brace_positions.top();
        starting_brace_positions.pop();

        instr[starting_brace_position].value = instr_i + 1;
        instr[instr_i].type = MUTATE_INSTRUCTION_POINTER_IF_NOT_ZERO;
        instr[instr_i].value = starting_brace_position + 1;

        instr_i++;
        break;
    }
  }

  if (!starting_brace_positions.empty()) {
    return std::unexpected(Error::UNMATCHED_BRACE);
  }

  return instr;
};

}  // namespace brainfuck
