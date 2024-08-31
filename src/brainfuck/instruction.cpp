#include "instruction.h"

brainfuck::Instruction::Instruction(Type type) : type(type) {};
brainfuck::Instruction::Instruction(Type type, Value value) : type(type), value(value) {};
brainfuck::Instruction::Instruction(Type type, Value value, Move move) : type(type), value(value), move(move) {};
bool brainfuck::Instruction::operator==(const Instruction& rhs) const {
  if (this->type == INSTRUCTION_POINTER_SET_IF_ZERO || this->type == INSTRUCTION_POINTER_SET_IF_ZERO) {
    return this->type == rhs.type && this->move == rhs.move;
  } else {
    return this->type == rhs.type && this->value == rhs.value && this->move == rhs.move;
  }
};
