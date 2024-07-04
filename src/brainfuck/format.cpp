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
    default:
      return '?';
  }
}

std::string brainfuck::formatInstruction(const brainfuck::Instruction instruction) {
  const char type = brainfuck::formatInstructionType(instruction.type);
  if (instruction.type == DATA_TRANSFER_META) {
    return std::format("({:04}) += (0)x{:04}", instruction.offset, instruction.value);
  }
  return std::format("> {:04}, {} {:04}", instruction.move, type, instruction.value);
}

std::string brainfuck::formatInstruction(const brainfuck::Instruction instruction, const int instructionPointer) {
  return std::format("({:04}) ", instructionPointer) + brainfuck::formatInstruction(instruction);
}
