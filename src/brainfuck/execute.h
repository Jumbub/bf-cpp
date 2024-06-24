#pragma once

#include "types.h"

namespace brainfuck {

__attribute__((hot)) Error execute(ByteCode instructions);

}  // namespace brainfuck
