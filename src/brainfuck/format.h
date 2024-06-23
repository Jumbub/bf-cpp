#pragma once

#include "parse.h"

namespace brainfuck {

char formatInstructionType(const Type type);
std::string formatInstruction(const Instruction instruction, const int instruction_pointer = -1);

}  // namespace brainfuck
