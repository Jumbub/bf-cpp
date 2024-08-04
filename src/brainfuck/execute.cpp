#include "execute.h"

#include <iostream>
#include <unordered_map>

namespace brainfuck {

class InfiniteTape /* ~37 zettabytes in each direction */ {
  static constexpr int64_t TAPE_SIZE = 4096;
  std::unordered_map<int64_t, int64_t[TAPE_SIZE]> tapes;
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
  int64_t& reference() { return tapes[tapeIndex][cellIndex]; }
  int64_t& referenceAt(const int64_t offset) {
    auto localCellIndex = cellIndex;
    auto localTapeIndex = tapeIndex;
    moveCellAndTape(offset, localCellIndex, localTapeIndex);
    return tapes[localTapeIndex][localCellIndex];
  }
  char value() { return static_cast<char>(tapes[tapeIndex][cellIndex] & 255); }
  void move(int64_t move) { moveCellAndTape(move, cellIndex, tapeIndex); }
};

void output(const char output, const Value times) {
  for (int i = 0; i < times; i++) {
    std::cout << static_cast<char>(output);
  }
}

void input(int64_t& character, const Value times) {
  for (int i = 0; i < times; i++) {
    char input;
    std::cin >> std::noskipws >> input;
    if (!std::cin.eof()) {
      character = input;
    }
  }
}

void setupInstructionAddresses(const Instruction* begin, const Instruction* end, const void* jumpTable[]) {
  Instruction* current = const_cast<Instruction*>(begin);
  while (current < end) {
    if (current->type == DATA_POINTER_ADD) {
      throw std::runtime_error("Un-implemented instruction type");
    }
    if (current->type == DATA_TRANSFER) {
      current->next = current + current->value;
    }
    if (current->type == INSTRUCTION_POINTER_SET_IF_NOT_ZERO || current->type == INSTRUCTION_POINTER_SET_IF_ZERO) {
      current->next = const_cast<Instruction*>(begin + current->value);
    }
    current->jump = const_cast<void*>(jumpTable[current->type]);
    current++;
  }
}

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
      &&DATA_TRANSFER,
  };
  setupInstructionAddresses(begin, end, jumpTable);

  InfiniteTape tape;
  // int64_t datas[30000] = {0};
  // int64_t* data = &datas[0];
  Instruction* instruction = const_cast<Instruction*>(begin);

  tape.move(instruction->move);
  goto*(instruction->jump);

NEXT: {
  instruction++;
  tape.move(instruction->move);

  goto*(instruction->jump);
}

DATA_ADD: {
  tape.reference() += instruction->value;

  goto NEXT;
}

DATA_TRANSFER: {
  const auto multiplier = tape.value();
  const auto last = instruction->next;
  while (instruction < last) {
    instruction++;
    tape.referenceAt(instruction->move) += multiplier * instruction->value;
  }
  tape.reference() = 0;

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
  output(tape.value(), instruction->value);

  goto NEXT;
}

DATA_SET_FROM_INPUT: {
  input(tape.reference(), instruction->value);

  goto NEXT;
}

DONE:
};

}  // namespace brainfuck
