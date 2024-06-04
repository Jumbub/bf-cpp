#include "go.h"

#include <iostream>
#include "execute.h"
#include "file.h"
#include "parse.h"

namespace brainfuck {

void go(const char* filename) {
  const auto data = brainfuck::read(filename);
  if (!data.has_value()) {
    std::cout << "bad filename" << std::endl;
    return;
  }
  const auto instructions = brainfuck::parse(data.value());
  brainfuck::execute(instructions);
}

}  // namespace brainfuck
