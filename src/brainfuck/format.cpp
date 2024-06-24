#include "format.h"

#include <format>

__attribute__((cold)) char brainfuck::formatInstructionType(const brainfuck::Type type) {
  switch (type) {
    case NOOP:
      return '_';
    case DONE:
      return '0';
    case DATA_ADD:
      return '+';
    case DATA_SET:
      return '=';
    case DATA_RESET:
      return 'R';
    case DATA_TRANSFER:
      return '~';
    case DATA_MULTIPLY:
      return 'x';
    case DATA_MULTIPLY_AND_DIVIDE:
      return '*';
    case DATA_SET_FROM_INPUT:
      return ',';
    case DATA_PRINT:
      return '.';
    case DATA_POINTER_ADD:
      return '>';
    case DATA_POINTER_ADD_WHILE_NOT_ZERO:
      return '^';
    case INSTRUCTION_POINTER_SET_IF_ZERO:
      return '[';
    case INSTRUCTION_POINTER_SET_IF_NOT_ZERO:
      return ']';
    default:
      throw std::runtime_error("error bad data given to formatter");
  }
}

__attribute__((cold)) std::string brainfuck::formatInstruction(
    const brainfuck::Instruction instruction,
    const int instructionPointer) {
  const std::string base = instructionPointer == -1 ? "" : std::format("({:04}) ", instructionPointer);
  const char type = brainfuck::formatInstructionType(instruction.type);
  const auto instr = std::format("{} {:04} [{:04}]", type, instruction.value, instruction.offset);
  return base + instr;
}
