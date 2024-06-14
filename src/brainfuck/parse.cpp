#include "parse.h"

#include <algorithm>
#include <stack>

namespace brainfuck {

template <Type type, Value increment, char character>
void emplaceCumulativeInstruction(ByteCode& instr, const Code& code, size_t& code_i) {
  Value value = increment;
  while (code[code_i + 1] == character) {
    value += increment;
    code_i++;
  }

  Offset offset = 0;

  instr.emplace_back(type, value, offset);
}

inline void openBrace(ByteCode& instr, const Code& code, size_t& code_i, std::stack<size_t>& starting_brace_positions) {
  if (code[code_i + 2] == ']') {
    const auto middle = code[code_i + 1];
    if (middle == '-' || middle == '+') {
      instr.emplace_back(DATA_SET, 0);
      code_i += 2;
      return;
    }
  }

  instr.emplace_back(INSTRUCTION_POINTER_SET_IF_ZERO, 0);
  const auto currentIndex = instr.size() - 1;
  starting_brace_positions.push(currentIndex);
}

inline void closeBrace(
    ByteCode& instr,
    [[maybe_unused]] const Code& code,
    [[maybe_unused]] size_t& code_i,
    std::stack<size_t>& starting_brace_positions) {
  const size_t starting_brace_position = starting_brace_positions.top();
  starting_brace_positions.pop();

  instr.emplace_back(INSTRUCTION_POINTER_SET_IF_NOT_ZERO, starting_brace_position + 1);

  const auto currentIndex = instr.size() - 1;
  instr[starting_brace_position].value = static_cast<Value>(currentIndex) + 1;
}

struct CharacterLookups {
  bool characterWhitelist[256]{false};
  CharacterLookups() {
    characterWhitelist[static_cast<size_t>('+')] = true;
    characterWhitelist[static_cast<size_t>('-')] = true;
    characterWhitelist[static_cast<size_t>('>')] = true;
    characterWhitelist[static_cast<size_t>('<')] = true;
    characterWhitelist[static_cast<size_t>('[')] = true;
    characterWhitelist[static_cast<size_t>(']')] = true;
    characterWhitelist[static_cast<size_t>('.')] = true;
    characterWhitelist[static_cast<size_t>(',')] = true;
  }
};

[[nodiscard]] inline const Code cleanCode(const Code rawCode) noexcept {
  static CharacterLookups characterLookups;

  std::vector<char> code;
  code.reserve(rawCode.size());
  std::remove_copy_if(rawCode.begin(), rawCode.end(), std::back_inserter(code), [](const char character) {
    return !characterLookups.characterWhitelist[static_cast<size_t>(character)];
  });

  return code;
}

std::expected<ByteCode, Error> parse(const Code rawCode) {
  const auto code = cleanCode(rawCode);

  ByteCode instr;
  const auto size = code.size();
  instr.reserve(size + size % 2);
  instr.emplace_back(NOOP);

  std::stack<size_t> starting_brace_positions;

  for (size_t code_i = 0; code_i < code.size(); code_i++) {
    if (code[code_i] == '>') {
      emplaceCumulativeInstruction<DATA_POINTER_ADD, 1, '>'>(instr, code, code_i);
    } else if (code[code_i] == '<') {
      emplaceCumulativeInstruction<DATA_POINTER_ADD, -1, '<'>(instr, code, code_i);
    } else if (code[code_i] == '[') {
      openBrace(instr, code, code_i, starting_brace_positions);
    } else if (code[code_i] == ']') {
      if (starting_brace_positions.empty())
        return std::unexpected(Error::UNMATCHED_BRACE);
      closeBrace(instr, code, code_i, starting_brace_positions);
    } else if (code[code_i] == '+') {
      emplaceCumulativeInstruction<DATA_ADD, 1, '+'>(instr, code, code_i);
    } else if (code[code_i] == '-') {
      emplaceCumulativeInstruction<DATA_ADD, -1, '-'>(instr, code, code_i);
    } else if (code[code_i] == '.') {
      emplaceCumulativeInstruction<DATA_PRINT, 1, '.'>(instr, code, code_i);
    } else if (code[code_i] == ',') {
      emplaceCumulativeInstruction<DATA_SET_FROM_INPUT, 1, ','>(instr, code, code_i);
    }
  }

  if (!starting_brace_positions.empty()) {
    return std::unexpected(Error::UNMATCHED_BRACE);
  }

  return instr;
};

Instruction::Instruction(Type type) : type(type), value(0), offset(0){};
Instruction::Instruction(Type type, Value value) : type(type), value(value), offset(0){};
Instruction::Instruction(Type type, Value value, Offset offset) : type(type), value(value), offset(offset){};

}  // namespace brainfuck
