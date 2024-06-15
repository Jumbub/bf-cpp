#include "parse.h"

#include <algorithm>
#include <map>
#include <stack>

namespace brainfuck {

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

template <char character>
void handleOffsetInstructions(ByteCode& instr, const Code& code, size_t& code_i) noexcept {
  constexpr Type type = instructionForCharacter(character);
  constexpr Value increment = incrementForCharacter(character);

  Offset offset = accumulate<Offset, increment, character>(code, code_i);

  auto& last = instr.back();

  if (last.type == type) {
    last.offset += offset;
    if (last.offset == 0) {
      // >+< :: (+ offset)>< :: (+ offset)
      instr.pop_back();
    }
    return;
  }

  instr.emplace_back(type, 0, offset);
}

template <char character>
void handleValueInstructions(ByteCode& instr, const Code& code, size_t& code_i) {
  constexpr Type type = instructionForCharacter(character);
  constexpr Value increment = incrementForCharacter(character);

  Value value = accumulate<Value, increment, character>(code, code_i);

  auto& last = instr.back();

  // >+ :: (+ offset)>
  if (last.type == DATA_POINTER_ADD) {
    last.type = type;
    last.value = value;
    instr.emplace_back(DATA_POINTER_ADD, 0, last.offset);
    return;
  }

  instr.emplace_back(type, value);
}

inline void openBrace(
    ByteCode& instr,
    [[maybe_unused]] const Code& code,
    [[maybe_unused]] size_t& code_i,
    std::stack<size_t>& startingBracePosition) {
  instr.emplace_back(INSTRUCTION_POINTER_SET_IF_ZERO, 0);
  const auto currentIndex = instr.size() - 1;
  startingBracePosition.push(currentIndex);
}

inline void closeBrace(
    ByteCode& instr,
    [[maybe_unused]] const Code& code,
    [[maybe_unused]] size_t& code_i,
    std::stack<size_t>& startingBracePosition) {
  const size_t starting_brace_position = startingBracePosition.top();
  startingBracePosition.pop();

  auto& thirdLast = *std::prev(instr.end(), 3);
  auto& secondLast = *std::prev(instr.end(), 2);
  const auto& last = *std::prev(instr.end(), 1);

  // [-] :: (set 0)
  if (secondLast.type == INSTRUCTION_POINTER_SET_IF_ZERO && last.type == DATA_ADD && last.offset == 0 &&
      (last.value == 1 || last.value == -1)) {
    instr.pop_back();

    if (thirdLast.type == DATA_POINTER_ADD) {
      thirdLast.type = DATA_SET;
      thirdLast.value = 0;
      secondLast.type = DATA_POINTER_ADD;
      secondLast.value = 0;
      secondLast.offset = thirdLast.offset;
      return;
    }

    secondLast.type = DATA_SET;
    secondLast.value = 0;
    secondLast.offset = 0;
  } else {
    instr.emplace_back(INSTRUCTION_POINTER_SET_IF_NOT_ZERO, starting_brace_position + 1);
    instr[starting_brace_position].value = static_cast<Value>(instr.size());
  }
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
  instr.emplace_back(NOOP);
  instr.emplace_back(NOOP);

  std::stack<size_t> startingBracePosition;

  for (size_t code_i = 0; code_i < code.size(); code_i++) {
    if (code[code_i] == '>') {
      handleOffsetInstructions<'>'>(instr, code, code_i);
    } else if (code[code_i] == '<') {
      handleOffsetInstructions<'<'>(instr, code, code_i);
    } else if (code[code_i] == '[') {
      openBrace(instr, code, code_i, startingBracePosition);
    } else if (code[code_i] == ']') {
      if (startingBracePosition.empty())
        return std::unexpected(Error::UNMATCHED_BRACE);
      closeBrace(instr, code, code_i, startingBracePosition);
    } else if (code[code_i] == '+') {
      handleValueInstructions<'+'>(instr, code, code_i);
    } else if (code[code_i] == '-') {
      handleValueInstructions<'-'>(instr, code, code_i);
    } else if (code[code_i] == '.') {
      handleValueInstructions<'.'>(instr, code, code_i);
    } else if (code[code_i] == ',') {
      handleValueInstructions<','>(instr, code, code_i);
    }
  }

  if (!startingBracePosition.empty()) {
    return std::unexpected(Error::UNMATCHED_BRACE);
  }

  return instr;
};

Instruction::Instruction(Type type) : type(type), value(0), offset(0){};
Instruction::Instruction(Type type, Value value) : type(type), value(value), offset(0){};
Instruction::Instruction(Type type, Value value, Offset offset) : type(type), value(value), offset(offset){};

}  // namespace brainfuck
