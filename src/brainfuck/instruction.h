#pragma once

#include <cstdint>

namespace brainfuck {

enum Type : uint64_t {
  NOOP,
  DONE,
  DATA_ADD,                             // + // -
  DATA_SET,                             // [-]
  DATA_RESET,                           // [-]
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
using Jump = void*;  // Stores labelled gotos. See `execute.cpp`.

static_assert(sizeof(void*) == sizeof(Type));

struct alignas(16) Instruction {
  union {
    Type type = NOOP;
    void* jump;
  };
  union {
    Value value = 0;
    Instruction* next;
  };
  Offset offset = 0;

  Instruction(Type type);
  Instruction(Type type, Value value);
  Instruction(Type type, Value value, Offset offset);
};

}  // namespace brainfuck
