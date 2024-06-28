#include "go.h"

#include "execute.h"
#include "file.h"
#include "parse.h"

namespace brainfuck {

Result go(const std::string filename) {
  const auto readResult = read(filename);
  if (!readResult) {
    return Result::PROGRAM_NOT_FOUND;
  }

  const auto parseResult = parse(readResult.value());
  if (!parseResult) {
    return Result::UNMATCHED_BRACE;
  }

  execute(parseResult.value());

  return Result::DONE;
}

}  // namespace brainfuck
