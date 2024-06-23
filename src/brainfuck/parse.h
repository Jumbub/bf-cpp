#pragma once

#include "types.h"

namespace brainfuck {

std::expected<ByteCode, Error> parse(Code full);

}  // namespace brainfuck
