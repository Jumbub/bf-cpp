#include "format.h"

#include <format>

char brainfuck::formatInstructionType(const brainfuck::Type type) {
  switch (type) {
    case NOOP:
      return '_';
    case DONE:
      return '0';
    case DATA_ADD:
      return '+';
    case DATA_SET_FROM_INPUT:
      return ',';
    case DATA_PRINT:
      return '.';
    case DATA_POINTER_ADD:
      return '>';
    case INSTRUCTION_POINTER_SET_IF_ZERO:
      return '[';
    case INSTRUCTION_POINTER_SET_IF_NOT_ZERO:
      return ']';
    case DATA_TRANSFER:
      return 'T';
    case DATA_TRANSFER_META:
      return 't';
    case DATA_SET:
      return '=';
    case DATA_SCAN:
      return '^';
    default:
      return '?';
  }
}

std::string brainfuck::formatInstruction(const brainfuck::Instruction instruction) {
  const char type = brainfuck::formatInstructionType(instruction.type);
  return std::format("> {:04}, {} {:04}", instruction.move, type, instruction.value);
}

std::string brainfuck::formatInstruction(const brainfuck::Instruction instruction, const int instruction_pointer) {
  const char type = brainfuck::formatInstructionType(instruction.type);
  if (instruction.type == INSTRUCTION_POINTER_SET_IF_NOT_ZERO) {
    return std::format("> {:04}, {} {:04}", instruction.move, type, instruction.value - instruction_pointer);
  }
  return std::format("> {:04}, {} {:04}", instruction.move, type, instruction.value);
}
