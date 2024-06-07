#pragma once

#include <string>
#include "execute.h"

namespace brainfuck {

template <
    Config config = DefaultConfig,
    std::integral Data = DefaultData,
    std::integral DataPointer = DefaultDataPointer,
    std::integral InstructionPointer = DefaultInstructionPointer>
int go(const std::string filename);

}  // namespace brainfuck
