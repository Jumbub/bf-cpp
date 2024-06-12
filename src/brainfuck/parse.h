#pragma once

#include <expected>
#include <format>
#include <iostream>
#include <string>
#include <vector>
#include "error.h"

namespace brainfuck {

enum Type : uint8_t {
  NOOP = '_',
  READ = ',',
  WRITE = '.',
  MUTATE_DATA = '+',
  MUTATE_DATA_POINTER = '>',
  MUTATE_INSTRUCTION_POINTER_IF_ZERO = '[',
  MUTATE_INSTRUCTION_POINTER_IF_NOT_ZERO = ']',

  SET = '=',  // [-]
};

using Value = int32_t;
struct Instruction {
  Type type = NOOP;
  Value value = 0;

  Instruction(Type type, Value value);
};

using ByteCode = std::vector<Instruction>;
using Code = std::vector<char>;

std::expected<ByteCode, Error> parse(Code full);

}  // namespace brainfuck
