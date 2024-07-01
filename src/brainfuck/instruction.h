#pragma once

#include <cstdint>

namespace brainfuck {

using Offset = int64_t;
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
  DATA_TRANSFER,                        // [-] // [->+<] // [->++>+++<<]
  DATA_POINTER_ADD_WHILE_NOT_ZERO,      // [>]
};

struct Instruction {
  union {
    Type type = NOOP;
    void* jump;     // Types are converted to labelled gotos at runtime.
    Offset offset;  // Some instructions are just containers for metadata.
  };
  union {
    Value value = 0;
    Instruction* next;  // Some values are converted to instruction pointers at runtime.
  };

  Instruction(Type type);
  Instruction(Type type, Value value);
  Instruction(Offset offset, Value value);
};

}  // namespace brainfuck
