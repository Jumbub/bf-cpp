#pragma once

#include "execute.h"
#include "file.h"
#include "parse.h"

namespace brainfuck {

__attribute__((hot)) Error go(const std::string filename) {
  const auto data = brainfuck::read(filename);
  if (!data) {
    return Error::PROGRAM_NOT_FOUND;
  }

  const auto instructions = brainfuck::parse(data.value());
  if (!instructions) {
    return instructions.error();
  }

  return execute(*instructions);
}

}  // namespace brainfuck
