#pragma once

#include <string>
#include "parse.h"

namespace brainfuck {

__attribute__((cold)) char formatInstructionType(const Type type);
__attribute__((cold)) std::string formatInstruction(const Instruction instruction, const int instruction_pointer = -1);

}  // namespace brainfuck
