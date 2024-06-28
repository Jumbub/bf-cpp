#pragma once

#include <string>
#include "parse.h"

namespace brainfuck {

char formatInstructionType(const Type type);
std::string formatInstruction(const Instruction instruction);
std::string formatInstruction(const Instruction instruction, const int instruction_pointer);

}  // namespace brainfuck
