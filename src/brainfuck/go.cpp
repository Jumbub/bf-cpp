#include "go.h"

#include "execute.h"
#include "file.h"
#include "parse.h"

namespace brainfuck {

int go(const std::string filename) {
  const auto data = brainfuck::read(filename);
  if (!data.has_value()) {
    std::cout << "bad filename" << std::endl;
    return EXIT_FAILURE;
  }
  const auto instructions = brainfuck::parse(data.value());
  brainfuck::execute(instructions);
  return EXIT_SUCCESS;
}

}  // namespace brainfuck
