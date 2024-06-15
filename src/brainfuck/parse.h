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

  // [->++<] :: (multiplyer)(add -1)(add 2 at offset 1)
  DATA_SET_MULTIPLE_MANY = '*',

  DATA_SET_FROM_INPUT = ',',
  DATA_PRINT = '.',

  DATA_POINTER_ADD = '>',
  DATA_POINTER_ADD_WHILE_NOT_ZERO = '^',

  INSTRUCTION_POINTER_SET_IF_ZERO = '[',
  INSTRUCTION_POINTER_SET_IF_NOT_ZERO = ']',
};

using Value = int32_t;
using Offset = int32_t;

struct Instruction {
  Type type = NOOP;
  Value value = 0;
  Offset offset = 0;

  Instruction(Type type);
  Instruction(Type type, Value value);
  Instruction(Type type, Value value, Offset offset);
};

using ByteCode = std::vector<Instruction>;
using Code = std::vector<char>;

std::expected<ByteCode, Error> parse(Code full);

}  // namespace brainfuck
