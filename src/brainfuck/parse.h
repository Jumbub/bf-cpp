#pragma once

#include <optional>
#include <vector>
#include "instruction.h"

namespace brainfuck {

std::optional<std::vector<Instruction>> parse(std::vector<char> source);

}  // namespace brainfuck
