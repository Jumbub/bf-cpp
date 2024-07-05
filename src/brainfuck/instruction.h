#pragma once

#include <cstdint>

namespace brainfuck {

using Move = int64_t;
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
  DATA_TRANSFER_META,                   //
  DATA_POINTER_ADD_WHILE_NOT_ZERO,      // [>] // [<<]
};

struct Instruction {
  union {
    Type type = NOOP;
    void* jump;  // Types are converted to labelled gotos at runtime.
  };
  union {
    Value value = 0;
    Instruction* next;  // Some values are converted to instruction pointers at runtime.
  };
  Move move = 0;

  Instruction(Type type);
  Instruction(Type type, Value value);
  Instruction(Type type, Value value, Move move);
};

}  // namespace brainfuck
