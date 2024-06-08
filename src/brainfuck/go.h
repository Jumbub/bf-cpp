#pragma once

#include "execute.h"
#include "file.h"

namespace brainfuck {

template <
    Config CONFIG = DefaultConfig,
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
  return brainfuck::execute<CONFIG, Data, DataPointer, InstructionPointer>(instructions);
}

}  // namespace brainfuck
