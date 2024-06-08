#pragma once

#include <expected>
#include <format>
#include <iostream>
#include <string>
#include <vector>
#include "error.h"

namespace brainfuck {

enum Type {
  NOOP = 'N',
  READ = ',',
  WRITE = '.',
  MUTATE_DATA = '+',
  MUTATE_DATA_POINTER = '>',
  MUTATE_INSTRUCTION_POINTER_IF_ZERO = '[',
  MUTATE_INSTRUCTION_POINTER_IF_NOT_ZERO = ']'
};

struct Instruction {
  Type type = NOOP;
  int value = 0;
};

using Instructions = std::vector<Instruction>;

std::expected<Instructions, Error> parse(std::vector<char> full);

}  // namespace brainfuck
