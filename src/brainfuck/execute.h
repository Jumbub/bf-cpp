#pragma once

#include <concepts>
#include <vector>
#include "parse.h"

namespace brainfuck {

enum EOFBehaviour { SET_ZERO, SET_NEGATIVE_ONE, EOF_NOOP };
enum DataPointerOverflowBehaviour { UNDEFINED };
struct Config {
  const uint64_t data_size = 30000;
  const EOFBehaviour eof_behaviour = EOF_NOOP;
  const DataPointerOverflowBehaviour data_pointer_overflow_behaviour = UNDEFINED;
};

using DefaultData = char;
using DefaultDataPointer = uint64_t;
using DefaultInstructionPointer = uint64_t;
constexpr Config DefaultConfig{};

template <
    Config config = DefaultConfig,
    std::integral Data = DefaultData,
    std::integral DataPointer = DefaultDataPointer,
    std::integral InstructionPointer = DefaultInstructionPointer>
int execute(Instructions instructions);

}  // namespace brainfuck
