#include "go.h"

#include "file.h"

namespace brainfuck {

template <Config config, std::integral Data, std::integral DataPointer, std::integral InstructionPointer>
int go(const std::string filename) {
  const auto data = brainfuck::read(filename);
  if (!data.has_value()) {
    std::cerr << "bad filename" << std::endl;
    return EXIT_FAILURE;
  }
  const auto instructions = brainfuck::parse(data.value());
  /* return brainfuck::execute<Config, Data, DataPointer, InstructionPointer>(instructions); */
}

}  // namespace brainfuck
