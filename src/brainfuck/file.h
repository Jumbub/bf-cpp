#pragma once

#include <optional>
#include <string>
#include "types.h"

namespace brainfuck {

std::optional<Code> read(const std::string filename);

}  // namespace brainfuck
