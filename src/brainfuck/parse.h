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

  DATA_ADD = '+',
  DATA_SET = '=',
  DATA_SET_FROM_INPUT = ',',
  DATA_PRINT = '.',

  DATA_POINTER_ADD = '>',

  INSTRUCTION_POINTER_SET_IF_ZERO = '[',
  INSTRUCTION_POINTER_SET_IF_NOT_ZERO = ']',
};

using Value = int32_t;
using Offset = int32_t;

struct Instruction {
  Type type = NOOP;
  Value value = 0;
  Offset offset = 0;

  Instruction(Type type, Value value);
};

using ByteCode = std::vector<Instruction>;
using Code = std::vector<char>;

std::expected<ByteCode, Error> parse(Code full);

}  // namespace brainfuck
