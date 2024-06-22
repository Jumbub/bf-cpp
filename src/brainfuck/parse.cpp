#include "parse.h"

#include <algorithm>
#include <stack>
#include <stdexcept>

namespace brainfuck {

using OpenBraceIterators = std::stack<std::vector<Instruction>::iterator>;

template <typename T, T increment, char character>
[[nodiscard]] inline T accumulate(const Code& code, size_t& code_i) noexcept {
  T accumulated = increment;
  while (code[code_i + 1] == character) {
    accumulated += increment;
    code_i++;
  }
  return accumulated;
}

consteval Value incrementForCharacter(const char character) {
  if (character == '.') {
    return 1;
  } else if (character == ',') {
    return 1;
  } else if (character == '+') {
    return 1;
  } else if (character == '-') {
    return -1;
  } else if (character == '>') {
    return 1;
  } else if (character == '<') {
    return -1;
  } else if (character == '[') {
    return 0;
  } else if (character == ']') {
    return 0;
  } else {
    throw std::runtime_error("invalid character");
  }
}

consteval Type instructionForCharacter(const char character) {
  if (character == '.') {
    return DATA_PRINT;
  } else if (character == ',') {
    return DATA_SET_FROM_INPUT;
  } else if (character == '+' || character == '-') {
    return DATA_ADD;
  } else if (character == '<' || character == '>') {
    return DATA_POINTER_ADD;
  } else if (character == '[') {
    return INSTRUCTION_POINTER_SET_IF_ZERO;
  } else if (character == ']') {
    return INSTRUCTION_POINTER_SET_IF_NOT_ZERO;
  } else {
    throw std::runtime_error("invalid character");
  }
}

constexpr bool isDataAdd(const Instruction& instruction) {
  return instruction.type == DATA_ADD;
};

template <char character>
void handleValueInstructions(ByteCode& instr, const Code& code, size_t& code_i) {
  constexpr Type type = instructionForCharacter(character);
  constexpr Value increment = incrementForCharacter(character);

  Value value = accumulate<Value, increment, character>(code, code_i);

  instr.emplace_back(type, value);
}

inline void openBrace(ByteCode& instr, OpenBraceIterators& openBraceIterators) {
  instr.emplace_back(INSTRUCTION_POINTER_SET_IF_ZERO, -123456789);
  openBraceIterators.push(std::prev(instr.end()));
}

inline void closeBrace(ByteCode& instr, OpenBraceIterators& openBraceIterators) {
  openBraceIterators.pop();
  instr.emplace_back(INSTRUCTION_POINTER_SET_IF_NOT_ZERO);
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
  code.push_back('$');  // final character to allow lookaheads without boundary checks

  return code;
}

[[nodiscard]] inline const ByteCode removeNoops(const ByteCode rawInstr) noexcept {
  ByteCode instr;
  instr.reserve(rawInstr.size());
  std::remove_copy_if(rawInstr.begin(), rawInstr.end(), std::back_inserter(instr), [](const Instruction& instruction) {
    return instruction.type == NOOP;
  });
  return instr;
}

inline void applyLoopIndices(ByteCode& instr) noexcept {
  std::stack<std::tuple<size_t, Instruction*>> loops;
  for (size_t i = 0; i < instr.size(); i++) {
    if (instr[i].type == INSTRUCTION_POINTER_SET_IF_ZERO) {
      loops.push({i, &instr[i]});
    } else if (instr[i].type == INSTRUCTION_POINTER_SET_IF_NOT_ZERO) {
      const auto [startI, startInstr] = loops.top();
      startInstr->value = static_cast<Value>(i + 1);
      instr[i].value = static_cast<Value>(startI + 1);
      loops.pop();
    }
  }
}

std::expected<ByteCode, Error> parse(const Code rawCode) {
  const auto code = cleanCode(rawCode);

  ByteCode instr;
  const auto size = code.size();
  instr.reserve(size + size % 2);
  instr.emplace_back(NOOP);
  instr.emplace_back(NOOP);
  instr.emplace_back(NOOP);

  OpenBraceIterators openBraceIterators;

  for (size_t code_i = 0; code_i < code.size(); code_i++) {
    if (code[code_i] == '>') {
      handleValueInstructions<'>'>(instr, code, code_i);
    } else if (code[code_i] == '<') {
      handleValueInstructions<'<'>(instr, code, code_i);
    } else if (code[code_i] == '[') {
      openBrace(instr, openBraceIterators);
    } else if (code[code_i] == ']') {
      if (openBraceIterators.empty())
        return std::unexpected(Error::UNMATCHED_BRACE);
      closeBrace(instr, openBraceIterators);
    } else if (code[code_i] == '+') {
      handleValueInstructions<'+'>(instr, code, code_i);
    } else if (code[code_i] == '-') {
      handleValueInstructions<'-'>(instr, code, code_i);
    } else if (code[code_i] == '.') {
      handleValueInstructions<'.'>(instr, code, code_i);
    } else if (code[code_i] == ',') {
      handleValueInstructions<','>(instr, code, code_i);
    } else if (code[code_i] == '$') {
      instr.emplace_back(DONE);
    }
  }

  if (!openBraceIterators.empty()) {
    return std::unexpected(Error::UNMATCHED_BRACE);
  }

  auto cleanInstr = removeNoops(instr);
  applyLoopIndices(cleanInstr);
  return cleanInstr;
};

}  // namespace brainfuck
