#pragma once

#include <cstdint>
#include <vector>

namespace brainfuck {

enum class Error : int {
  NONE = 0,
  PROGRAM_NOT_FOUND,
  REACHED_INSTRUCTION_LIMIT,
  UNMATCHED_BRACE,
};

enum Type : uint64_t {
  NOOP,
  DONE,
  DATA_ADD,                             // + // -
  DATA_SET,                             // [-]
  DATA_TRANSFER,                        // [->+<]
  DATA_MULTIPLY,                        // [->++<]
  DATA_MULTIPLY_AND_DIVIDE,             // [-->+++<]
  DATA_SET_FROM_INPUT,                  // ,
  DATA_PRINT,                           // .
  DATA_POINTER_ADD,                     // > // <
  DATA_POINTER_ADD_WHILE_NOT_ZERO,      // [>] // [<]
  INSTRUCTION_POINTER_SET_IF_ZERO,      // [
  INSTRUCTION_POINTER_SET_IF_NOT_ZERO,  // ]
};

using Value = int64_t;
using Offset = int64_t;

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

}  // namespace brainfuck
