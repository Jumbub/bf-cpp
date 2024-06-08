#pragma once

#include "execute.h"
#include "file.h"

namespace brainfuck {

template <
    uint64_t DATA_SIZE = DEFAULT_DATA_SIZE,
    EOFBehaviour EOF_BEHAVIOUR = DEFAULT_EOF_BEHAVIOUR,
    DataPointerOverflowBehaviour DATA_POINTER_OVERFLOW_BEHAVIOUR = DEFAULT_DATA_POINTER_OVERFLOW_BEHAVIOUR,
    std::integral Data = DefaultData,
    std::integral DataPointer = DefaultDataPointer,
    std::integral InstructionPointer = DefaultInstructionPointer>
int go(const std::string filename) {
  const auto data = brainfuck::read(filename);
  if (!data.has_value()) {
    std::cerr << "bad filename" << std::endl;
    return EXIT_FAILURE;
  }
  const auto instructions = brainfuck::parse(data.value());
  return brainfuck::execute<
      DATA_SIZE, EOF_BEHAVIOUR, DATA_POINTER_OVERFLOW_BEHAVIOUR, Data, DataPointer, InstructionPointer>(instructions);
}

}  // namespace brainfuck
