#include "execute.h"

#include <iostream>
#include <unordered_map>

namespace brainfuck {

void setupInstructionAddresses(const Instruction* begin, const Instruction* end, const void* jumpTable[]) {
  Instruction* current = const_cast<Instruction*>(begin);
  while (current < end) {
    if (current->type == DATA_POINTER_ADD) {
      throw std::runtime_error("Un-implemented instruction type");
    }
    if (current->type == INSTRUCTION_POINTER_SET_IF_NOT_ZERO || current->type == INSTRUCTION_POINTER_SET_IF_ZERO) {
      current->next = const_cast<Instruction*>(begin + current->value);
    }
    current->jump = const_cast<void*>(jumpTable[current->type]);
    current++;
  }
}

class InfiniteTape /* ~37 zettabytes in each direction */ {
  static constexpr int64_t TAPE_SIZE = 4096;
  std::unordered_map<int64_t, char[TAPE_SIZE]> tapes;
  int64_t tapeIndex = 1000;
  int64_t cellIndex = 0;
  void moveCellAndTape(int64_t move, int64_t& cellIndex, int64_t& tapeIndex) {
    cellIndex += move;
    while (cellIndex < 0) {
      cellIndex += TAPE_SIZE;
      tapeIndex -= 1;
    }
    while (cellIndex >= TAPE_SIZE) {
      cellIndex -= TAPE_SIZE;
      tapeIndex += 1;
    }
  }

 public:
  char& reference() { return tapes[tapeIndex][cellIndex]; }
  char value() { return static_cast<char>(tapes[tapeIndex][cellIndex] & 255); }
  void move(int64_t move) { moveCellAndTape(move, cellIndex, tapeIndex); }
};

class TapeRecorder /* record actions to hash output */ : InfiniteTape {
 public:
  char& set(char value) { return InfiniteTape::reference() = value; }
  char& add(char value) { return InfiniteTape::reference() += value; }
  char value() { return InfiniteTape::value(); }
  void move(int64_t move) { InfiniteTape::move(move); }
};

void execute(const Instruction* begin, const Instruction* end) {
  const void* jumpTable[] = {
      &&NEXT,
      &&DONE,
      &&DATA_ADD,
      &&DATA_SET_FROM_INPUT,
      &&DATA_PRINT,
      nullptr,
      &&INSTRUCTION_POINTER_SET_IF_ZERO,
      &&INSTRUCTION_POINTER_SET_IF_NOT_ZERO,
  };
  setupInstructionAddresses(begin, end, jumpTable);

  TapeRecorder tape;
  Instruction* instruction = const_cast<Instruction*>(begin);

  tape.move(instruction->move);
  goto*(instruction->jump);

NEXT: {
  instruction++;
  tape.move(instruction->move);

  goto*(instruction->jump);
}

DATA_ADD: {
  tape.add(instruction->cValue);

  goto NEXT;
}

INSTRUCTION_POINTER_SET_IF_NOT_ZERO: {
  const auto while_not_zero = instruction->next == instruction;
  while (while_not_zero && tape.value() != 0) {
    tape.move(instruction->move);
  }

  if (tape.value() != 0) {
    instruction = instruction->next;
    tape.move(instruction->move);

    goto*(instruction->jump);
  }

  goto NEXT;
}

INSTRUCTION_POINTER_SET_IF_ZERO: {
  if (tape.value() == 0) {
    instruction = instruction->next;
    tape.move(instruction->move);

    goto*(instruction->jump);
  }

  goto NEXT;
}

DATA_PRINT: {
  for (int i = 0; i < instruction->value; i++) {
    std::cout << tape.value();
  }

  goto NEXT;
}

DATA_SET_FROM_INPUT: {
  for (int i = 0; i < instruction->value; i++) {
    char input;
    std::cin >> std::noskipws >> input;
    if (!std::cin.eof()) {
      tape.set(input);
    }
  }

  goto NEXT;
}

DONE:
};

}  // namespace brainfuck
