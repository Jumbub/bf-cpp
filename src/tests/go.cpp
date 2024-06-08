#include "../brainfuck/go.h"
#include "doctest/doctest.h"
#include "utils.h"

template <
    uint64_t DATA_SIZE = 30000,
    uint64_t ITERATION_LIMIT = 10000000000,
    brainfuck::EOFBehaviour EOF_BEHAVIOUR = brainfuck::EOFBehaviour::SET_ZERO,
    brainfuck::DataPointerOverflowBehaviour DATA_POINTER_OVERFLOW_BEHAVIOUR =
        brainfuck::DataPointerOverflowBehaviour::UNDEFINED,
    std::integral Data = char,
    std::integral DataPointer = uint64_t,
    std::integral InstructionPointer = uint64_t>
void runSnapshotTest(const std::string filename, const std::optional<std::string> input = std::nullopt) {
  const auto stopCapturingIO = startCapturingIO(input);

  try {
    const auto result = brainfuck::go<
        DATA_SIZE, ITERATION_LIMIT, EOF_BEHAVIOUR, DATA_POINTER_OVERFLOW_BEHAVIOUR, Data, DataPointer,
        InstructionPointer>("samples/" + filename);

    const auto output = stopCapturingIO();
    CHECK_MESSAGE(result == EXIT_SUCCESS, "Failed with error code: ", result);
    REQUIRE_SNAPSHOT("go/" + filename, output);
  } catch (...) {
    stopCapturingIO();
    throw;
  }
}

TEST_SUITE("go") {
  TEST_CASE("empty_file") {
    runSnapshotTest("tests/empty_file.b");
  }

  TEST_CASE("no_loop_hello") {
    runSnapshotTest("tests/no_loop_hello.b");
  }

  TEST_CASE("echo") {
    runSnapshotTest("echo.b", "wow this\nis amaze");
  }

  /* TEST_CASE("unmatched_brace") { */
  /*   runSnapshotTest("unmatched_brace.b"); */
  /* } */

  /* TEST_CASE("mandelbrot") { */
  /*   runSnapshotTest("mandelbrot.b", std::nullopt); */
  /* } */
}
