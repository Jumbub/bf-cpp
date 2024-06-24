#pragma once

#include <optional>
#include <string>
#include "types.h"

namespace brainfuck {

__attribute__((cold)) std::optional<Code> read(const std::string filename);

}  // namespace brainfuck
