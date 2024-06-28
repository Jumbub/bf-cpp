#pragma once

#include <expected>
#include "types.h"

namespace brainfuck {

std::expected<std::vector<Instruction>, Error> parse(std::vector<char> full);

}  // namespace brainfuck
