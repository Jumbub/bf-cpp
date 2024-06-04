#include "parse.h"

#include <stack>
#include <stdexcept>

namespace brainfuck {

template <typename First, typename... T>
bool is(First&& first, T&&... t) {
  return ((first == t) || ...);
}

std::vector<Instruction> parse(std::vector<char> full) {
  std::vector<Instruction> shorter(full.size());
  std::stack<int> starting_brace_positions;

  int shorter_i = 0;
  for (int i = 0; i < full.size(); i++) {
    switch (full[i]) {
      case '+':
        shorter[shorter_i].mutate_data = 1;
        shorter_i++;
        break;
      case '-':
        shorter[shorter_i].mutate_data = -1;
        shorter_i++;
        break;
      case '>':
        shorter[shorter_i].mutate_data_pointer = 1;
        shorter_i++;
        break;
      case '<':
        shorter[shorter_i].mutate_data_pointer = -1;
        shorter_i++;
        break;
      case '[':
        starting_brace_positions.push(shorter_i);
        shorter_i++;
        break;
      case '.':
        shorter[shorter_i].io = WRITE;
        shorter_i++;
        break;
      case ',':
        shorter[shorter_i].io = READ;
        shorter_i++;
        break;
      case ']':
        if (starting_brace_positions.empty())
          throw std::invalid_argument("brainfuck code has non-matching braces");
        const int starting_brace_position = starting_brace_positions.top();
        starting_brace_positions.pop();

        shorter[starting_brace_position].mutate_instruction_pointer_if[0] = shorter_i - starting_brace_position + 1;
        shorter[starting_brace_position].mutate_instruction_pointer_if[1] = 1;

        shorter[shorter_i].mutate_instruction_pointer_if[0] = 1;
        shorter[shorter_i].mutate_instruction_pointer_if[1] = starting_brace_position - shorter_i;

        shorter_i++;
        break;
    }
  }

  shorter.resize(shorter_i);
  return shorter;
};

}  // namespace brainfuck
