#pragma once

#include <string>

namespace brainfuck {

enum class Result : int {
  DONE = 0,
  PROGRAM_NOT_FOUND,
  UNMATCHED_BRACE,
};

Result go(const std::string filename);

}  // namespace brainfuck
