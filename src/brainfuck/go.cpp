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

  const auto parseResult = parse(*readResult);
  if (!parseResult) {
    return Result::UNMATCHED_BRACE;
  }

  execute(&*parseResult->begin(), &*parseResult->end());

  return Result::DONE;
}

}  // namespace brainfuck
