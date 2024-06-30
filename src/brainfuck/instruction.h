#pragma once

#include <cstdint>

namespace brainfuck {

using Value = int64_t;
enum Type : uint64_t {
  NOOP,                                 // foo
  DONE,                                 // EOF
  DATA_ADD,                             // + // -
  DATA_SET_FROM_INPUT,                  // ,
  DATA_PRINT,                           // .
  DATA_POINTER_ADD,                     // > // <
  INSTRUCTION_POINTER_SET_IF_ZERO,      // [
  INSTRUCTION_POINTER_SET_IF_NOT_ZERO,  // ]
};

struct alignas(16) Instruction {
  union {
    Type type = NOOP;
    void* jump;  // Types are converted to labelled gotos at runtime.
  };
  union {
    Value value = 0;
    Instruction* next;  // Some values are converted to instruction pointers at runtime.
  };

  Instruction(Type type);
  Instruction(Type type, Value value);
};

}  // namespace brainfuck
