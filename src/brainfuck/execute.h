#pragma once

#include "instruction.h"

namespace brainfuck {

Error execute(std::vector<Instruction> instructions);

}  // namespace brainfuck
