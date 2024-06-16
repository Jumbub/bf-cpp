#include "parse.h"

#include <algorithm>
#include <map>
#include <stack>

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

constexpr bool hasZeroOffset(const Instruction& instruction) {
  return instruction.offset == 0;
};

constexpr bool sortByOffsetUnlessZero(const Instruction& lhs, const Instruction& rhs) {
  return lhs.offset < rhs.offset || (lhs.offset == 0 && rhs.offset != 0);
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

inline void openBrace(ByteCode& instr, OpenBraceIterators& openBraceIterators) {
  instr.emplace_back(INSTRUCTION_POINTER_SET_IF_ZERO, -123456789);
  openBraceIterators.push(std::prev(instr.end()));
}

inline void closeBrace(ByteCode& instr, OpenBraceIterators& openBraceIterators) {
  const auto openBraceIterator = openBraceIterators.top();
  openBraceIterators.pop();

  auto& thirdLast = *std::prev(instr.end(), 3);
  auto& secondLast = *std::prev(instr.end(), 2);
  const auto& last = *std::prev(instr.cend(), 1);

  // [-
  if (secondLast.type == INSTRUCTION_POINTER_SET_IF_ZERO && last.type == DATA_ADD && last.offset == 0 &&
      (last.value == 1 || last.value == -1)) {
    instr.pop_back();

    if (thirdLast.type == DATA_POINTER_ADD) {
      // >[-] :: (set 0 at offset)>
      thirdLast.type = DATA_SET;
      thirdLast.value = 0;
      secondLast.type = DATA_POINTER_ADD;
      secondLast.value = 0;
      secondLast.offset = thirdLast.offset;
      return;
    } else {
      // [-] :: (set 0)
      secondLast.type = DATA_SET;
      secondLast.value = 0;
      secondLast.offset = 0;
      return;
    }
  }

  // [-+
  if (std::all_of(std::next(openBraceIterator), instr.end(), isDataAdd) &&
      std::any_of(std::next(openBraceIterator), instr.end(), hasZeroOffset)) {
    std::stable_sort(std::next(openBraceIterator), instr.end(), sortByOffsetUnlessZero);

    const auto zeroOffsetValue = std::next(openBraceIterator)->value;
    if (zeroOffsetValue == -1 && (std::distance(openBraceIterator, instr.end()) == 3) && instr.back().value == 1) {
      // [->+<] :: (transfer to offset)
      const auto moveTo = instr.back().offset;
      instr.pop_back();
      instr.pop_back();
      instr.back().type = DATA_TRANSFER;
      instr.back().value = moveTo;
      return;
    } else if (zeroOffsetValue == -1) {
      // [->++>+++<<] :: (multiply)(offset 1 multiply base by 2)(offset 1 multiply base by 3)
      openBraceIterator->type = DATA_MULTIPLY;
      openBraceIterator->value = static_cast<Value>(std::distance(openBraceIterator, instr.end()) - 2);
      openBraceIterator->offset = 0;
      instr.erase(std::next(openBraceIterator));
      return;
    } else {
      // [-->+>+++<<] :: (multiply & divide)(offset 1 multiply base by 1/2)(offset 1 multiply base by 3/2)
      // openBraceIterator->type = DATA_MULTIPLY_AND_DIVIDE;
      // openBraceIterator->value = static_cast<Value>(std::distance(openBraceIterator, instr.end()) - 2);
      // openBraceIterator->offset = 0;
      // return;
    }
    // todo shuffle pointer add
  }

  // [>
  if (secondLast.type == INSTRUCTION_POINTER_SET_IF_ZERO && last.type == DATA_POINTER_ADD) {
    if (thirdLast.type == DATA_POINTER_ADD) {
      // >[>] :: (scan for 0 at offset)>
      thirdLast.type = DATA_POINTER_ADD_WHILE_NOT_ZERO;
      thirdLast.value = last.offset;
      secondLast.type = DATA_POINTER_ADD;
      secondLast.value = 0;
      secondLast.offset = thirdLast.offset;
      instr.pop_back();
      return;
    } else {
      // [>] :: (scan for 0)
      secondLast.type = DATA_POINTER_ADD_WHILE_NOT_ZERO;
      secondLast.value = last.offset;
      secondLast.offset = 0;
      instr.pop_back();
      return;
    }
  }

  // regular loop
  const size_t i = static_cast<size_t>(std::distance(instr.begin(), openBraceIterator));
  instr.emplace_back(INSTRUCTION_POINTER_SET_IF_NOT_ZERO, std::distance(instr.begin(), openBraceIterator) + 1);
  instr[i].value = static_cast<Value>(instr.size());
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

  OpenBraceIterators openBraceIterators;

  for (size_t code_i = 0; code_i < code.size(); code_i++) {
    if (code[code_i] == '>') {
      handleOffsetInstructions<'>'>(instr, code, code_i);
    } else if (code[code_i] == '<') {
      handleOffsetInstructions<'<'>(instr, code, code_i);
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
    }
  }

  if (!openBraceIterators.empty()) {
    return std::unexpected(Error::UNMATCHED_BRACE);
  }

  return instr;
};

Instruction::Instruction(Type type) : type(type), value(0), offset(0){};
Instruction::Instruction(Type type, Value value) : type(type), value(value), offset(0){};
Instruction::Instruction(Type type, Value value, Offset offset) : type(type), value(value), offset(offset){};

}  // namespace brainfuck
