#include "utils.h"

#include <stdio.h>
#include <unistd.h>
#include <sstream>
#include "../brainfuck/go.h"
#include "doctest/doctest.h"

template <
    uint64_t DATA_SIZE = 30000,
    uint64_t ITERATION_LIMIT = 100000,
    brainfuck::EOFBehaviour EOF_BEHAVIOUR = brainfuck::EOFBehaviour::SET_ZERO,
    brainfuck::DataPointerOverflowBehaviour DATA_POINTER_OVERFLOW_BEHAVIOUR =
        brainfuck::DataPointerOverflowBehaviour::UNDEFINED,
    std::integral Data = char,
    std::integral DataPointer = uint64_t,
    std::integral InstructionPointer = uint64_t>
void runSnapshotTest(const std::string filename, const std::optional<std::string> input) {
  std::stringstream newStdout;
  std::stringstream newStdin;
  std::streambuf* oldStdout = std::cout.rdbuf(newStdout.rdbuf());
  std::streambuf* oldStdin = std::cin.rdbuf(newStdin.rdbuf());

  if (input.has_value()) {
    newStdin << input.value();
  }

  brainfuck::go<
      DATA_SIZE, ITERATION_LIMIT, EOF_BEHAVIOUR, DATA_POINTER_OVERFLOW_BEHAVIOUR, Data, DataPointer,
      InstructionPointer>("samples/" + filename);
  requireSnapshot("go/" + filename, newStdout.str());

  std::cout.rdbuf(oldStdout);
  std::cin.rdbuf(oldStdin);
}

TEST_SUITE("go") {
  TEST_CASE("empty_file") {
    runSnapshotTest("tests/empty_file.b", std::nullopt);
  }

  TEST_CASE("no_loop_hello") {
    runSnapshotTest("tests/no_loop_hello.b", std::nullopt);
  }

  TEST_CASE("echo") {
    runSnapshotTest("echo.b", "wow this\nis amaze");
  }
}
