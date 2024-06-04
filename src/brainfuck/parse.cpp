#include "parse.h"

#include <stack>
#include <stdexcept>

namespace brainfuck {

template <typename First, typename... T>
bool is(First&& first, T&&... t) {
  return ((first == t) || ...);
}

std::vector<Instruction> parse(std::vector<char> full) {
  std::vector<Instruction> shorter(full.size() + 1);
  std::stack<int> starting_brace_positions;

  int shorter_i = 1;  // operates with +1 offset to simplify optimisation logic
  for (int i = 0; i < full.size(); i++) {
    switch (full[i]) {
      case '+':
        if (shorter[shorter_i - 1].type == MUTATE_DATA) {
          shorter[shorter_i - 1].value += 1;
        } else {
          shorter[shorter_i].type = MUTATE_DATA;
          shorter[shorter_i].value = 1;
          shorter_i++;
        }
        break;
      case '-':
        if (shorter[shorter_i - 1].type == MUTATE_DATA) {
          shorter[shorter_i - 1].value -= 1;
        } else {
          shorter[shorter_i].type = MUTATE_DATA;
          shorter[shorter_i].value = -1;
          shorter_i++;
        }
        break;
      case '>':
        if (shorter[shorter_i - 1].type == MUTATE_DATA_POINTER) {
          shorter[shorter_i - 1].value += 1;
        } else {
          shorter[shorter_i].type = MUTATE_DATA_POINTER;
          shorter[shorter_i].value = 1;
          shorter_i++;
        }
        break;
      case '<':
        if (shorter[shorter_i - 1].type == MUTATE_DATA_POINTER) {
          shorter[shorter_i - 1].value -= 1;
        } else {
          shorter[shorter_i].type = MUTATE_DATA_POINTER;
          shorter[shorter_i].value = -1;
          shorter_i++;
        }
        break;
      case '.':
        shorter[shorter_i].type = WRITE;
        shorter_i++;
        break;
      case ',':
        shorter[shorter_i].type = READ;
        shorter_i++;
        break;
      case '[':
        starting_brace_positions.push(shorter_i);
        shorter_i++;
        break;
      case ']':
        if (starting_brace_positions.empty())
          throw std::invalid_argument("brainfuck code has non-matching braces");
        const int starting_brace_position = starting_brace_positions.top();
        starting_brace_positions.pop();

        shorter[starting_brace_position].type = MUTATE_INSTRUCTION_POINTER_IF_ZERO;
        shorter[starting_brace_position].value = shorter_i + 1;
        shorter[shorter_i].type = MUTATE_INSTRUCTION_POINTER_IF_NOT_ZERO;
        shorter[shorter_i].value = starting_brace_position + 1;

        shorter_i++;
        break;
    }
  }

  shorter.resize(shorter_i);
  return shorter;
};

}  // namespace brainfuck
