#pragma once

#include "execute.h"
#include "file.h"

namespace brainfuck {

template <
    uint64_t DATA_SIZE = 30000,
    uint64_t ITERATION_LIMIT = 100,
    EOFBehaviour EOF_BEHAVIOUR = EOFBehaviour::NOOP,
    DataPointerOverflowBehaviour DATA_POINTER_OVERFLOW_BEHAVIOUR = DataPointerOverflowBehaviour::UNDEFINED,
    std::integral Data = char,
    std::integral DataPointer = uint64_t,
    std::integral InstructionPointer = uint64_t>
int go(const std::string filename) {
  const auto data = brainfuck::read(filename);
  if (!data.has_value()) {
    std::cerr << "bad filename" << std::endl;
    return EXIT_FAILURE;
  }
  const auto instructions = brainfuck::parse(data.value());
  return brainfuck::execute<
      DATA_SIZE, ITERATION_LIMIT, EOF_BEHAVIOUR, DATA_POINTER_OVERFLOW_BEHAVIOUR, Data, DataPointer,
      InstructionPointer>(instructions);
}

}  // namespace brainfuck
