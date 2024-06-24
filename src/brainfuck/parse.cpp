#include "parse.h"

#include <algorithm>
#include <stack>
#include <stdexcept>

namespace brainfuck {

using OpenBraceIterators = std::stack<std::vector<Instruction>::iterator>;

template <typename T, T increment, char character>
__attribute__((cold)) [[nodiscard]] inline T accumulate(const Code& code, size_t& code_i) noexcept {
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

__attribute__((cold)) constexpr bool isDataAdd(const Instruction& instruction) {
  return instruction.type == DATA_ADD;
};

__attribute__((cold)) constexpr bool hasZeroOffset(const Instruction& instruction) {
  return instruction.offset == 0;
};

__attribute__((cold)) constexpr bool sortByOffsetUnlessZero(const Instruction& lhs, const Instruction& rhs) {
  if (lhs.offset == 0) {
    return true;
  }
  if (rhs.offset == 0) {
    return false;
  }
  return lhs.offset < rhs.offset;
}

// >... :: (... at offset)>
inline void checkAndApplySimplifications(ByteCode& instr, const ByteCode::iterator current) {
  if (std::prev(current)->type == DATA_POINTER_ADD) {
    const auto previous = std::prev(current);
    const auto offset = previous->offset;
    current->offset = offset;
    instr.erase(previous);
    instr.emplace_back(DATA_POINTER_ADD, 0, offset);
  }

  if (instr.back().type == DATA_POINTER_ADD) {
    const auto current = std::prev(instr.end(), 2);
    const auto previous = std::prev(instr.end(), 3);
    if (previous->type == DATA_SET && current->type == DATA_ADD && current->offset == previous->offset) {
      previous->value += current->value;
      instr.erase(current);
    }
  } else {
    const auto previous = std::prev(instr.end(), 2);
    if (previous->type == DATA_SET && current->type == DATA_ADD && current->offset == previous->offset) {
      previous->value += current->value;
      instr.pop_back();
    }
  }

  if (instr.back().type == DATA_SET && instr.back().value == 0) {
    const auto previous = std::prev(instr.end(), 2);
    if (previous->type == DATA_SET && previous->value == 0) {
      previous->value = instr.back().offset;
      previous->type = DATA_RESET;
      instr.erase(current);
    }
  }
}

template <char character>
__attribute__((cold)) void handleOffsetInstructions(ByteCode& instr, const Code& code, size_t& code_i) noexcept {
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
__attribute__((cold)) void handleValueInstructions(ByteCode& instr, const Code& code, size_t& code_i) {
  constexpr Type type = instructionForCharacter(character);
  constexpr Value increment = incrementForCharacter(character);

  Value value = accumulate<Value, increment, character>(code, code_i);

  instr.emplace_back(type, value);

  checkAndApplySimplifications(instr, std::prev(instr.end()));
}

inline void openBrace(ByteCode& instr, OpenBraceIterators& openBraceIterators) {
  instr.emplace_back(INSTRUCTION_POINTER_SET_IF_ZERO, -123456789);
  openBraceIterators.push(std::prev(instr.end()));
}

inline void closeBrace(ByteCode& instr, OpenBraceIterators& openBraceIterators) {
  const auto openBraceIterator = openBraceIterators.top();
  openBraceIterators.pop();

  auto& secondLast = *std::prev(instr.end(), 2);
  const auto& last = *std::prev(instr.cend(), 1);

  // [-
  if (secondLast.type == INSTRUCTION_POINTER_SET_IF_ZERO && last.type == DATA_ADD && last.offset == 0 &&
      (last.value == 1 || last.value == -1)) {
    instr.pop_back();

    // [-] :: (set 0)
    secondLast.type = DATA_SET;
    secondLast.value = 0;
    secondLast.offset = 0;

    checkAndApplySimplifications(instr, openBraceIterator);
    return;
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

      auto& last = instr.back();
      last.type = DATA_TRANSFER;
      last.value = moveTo;
      last.offset = 0;

      checkAndApplySimplifications(instr, openBraceIterator);
      return;
    } else {
      openBraceIterator->value = static_cast<Value>(std::distance(openBraceIterator, instr.end()) - 2);
      openBraceIterator->offset = 0;

      if (zeroOffsetValue == -1) {
        // [->++>+++<<] :: (multiply)(offset 1 multiply base by 2)(offset 1 multiply base by 3)
        openBraceIterator->type = DATA_MULTIPLY;
        instr.erase(std::next(openBraceIterator));
      } else {
        // [-->+>+++<<] :: (multiply & divide)(offset 1 multiply base by 1/2)(offset 1 multiply base by 3/2)
        openBraceIterator->type = DATA_MULTIPLY_AND_DIVIDE;
      }

      checkAndApplySimplifications(instr, openBraceIterator);
      return;
    }
    // todo shuffle pointer add
  }

  // [>
  if (secondLast.type == INSTRUCTION_POINTER_SET_IF_ZERO && last.type == DATA_POINTER_ADD) {
    // [>] :: (scan for 0)
    secondLast.type = DATA_POINTER_ADD_WHILE_NOT_ZERO;
    secondLast.value = last.offset;
    secondLast.offset = 0;
    instr.pop_back();

    checkAndApplySimplifications(instr, openBraceIterator);
    return;
  }

  // regular loop
  if (std::prev(openBraceIterator)->type == DATA_POINTER_ADD) {
    openBraceIterator->offset = std::prev(openBraceIterator)->offset;
    instr.erase(std::prev(openBraceIterator));
  }
  if (instr.back().type == DATA_POINTER_ADD) {
    instr.back().type = INSTRUCTION_POINTER_SET_IF_NOT_ZERO;
  } else {
    instr.emplace_back(INSTRUCTION_POINTER_SET_IF_NOT_ZERO);
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

__attribute__((cold)) std::expected<ByteCode, Error> parse(const Code rawCode) {
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
    } else if (code[code_i] == '$') {
      if (instr.back().type == DATA_POINTER_ADD) {
        instr.back().type = DONE;
      } else {
        instr.emplace_back(DONE);
      }
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
