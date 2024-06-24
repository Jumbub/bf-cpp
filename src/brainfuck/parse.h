#pragma once

#include <expected>
#include "types.h"

namespace brainfuck {

__attribute__((cold)) std::expected<ByteCode, Error> parse(Code full);

}  // namespace brainfuck
