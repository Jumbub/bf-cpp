#include "../brainfuck/go.h"
#include "doctest/doctest.h"
#include "utils.h"

using namespace brainfuck;

template <
    uint64_t DATA_SIZE = 30000,
    uint64_t ITERATION_LIMIT = 10000000000,
    EOFBehaviour EOF_BEHAVIOUR = EOFBehaviour::NOOP,
    DataPointerOverflowBehaviour DATA_POINTER_OVERFLOW_BEHAVIOUR = DataPointerOverflowBehaviour::UNDEFINED,
    std::integral Data = char,
    std::integral DataPointer = uint64_t,
    std::integral InstructionPointer = uint64_t>
void test(
    const std::string filename,
    const std::optional<std::string> input = std::nullopt,
    const Error expectedError = Error::NONE) {
  const auto stopCapturingIO = startCapturingIO(input);

  try {
    const auto error =
        go<DATA_SIZE, ITERATION_LIMIT, EOF_BEHAVIOUR, DATA_POINTER_OVERFLOW_BEHAVIOUR, Data, DataPointer,
           InstructionPointer>("samples/" + filename);

    const auto output = stopCapturingIO();
    CHECK_MESSAGE(error == expectedError, "Unexpected error code: ", error);
    REQUIRE_SNAPSHOT("go/" + filename, output);
  } catch (...) {
    stopCapturingIO();
    throw;
  }
}

TEST_SUITE("go") {
  TEST_CASE("empty_file") {
    const auto error = go("this-file-does-not-exist");
    REQUIRE(error == Error::PROGRAM_NOT_FOUND);
  }

  TEST_CASE("empty_file") {
    test("tests/empty_file.b");
  }

  TEST_CASE("no_loop_hello") {
    test("tests/no_loop_hello.b");
  }

  TEST_CASE("unmatched_brace_[") {
    test("tests/unmatched_brace_[.b");
  }

  TEST_CASE("unmatched_brace_]") {
    test("tests/unmatched_brace_].b", std::nullopt, Error::NONE_MATCHING_BRACES);
  }

  TEST_CASE("echo") {
    test("echo.b", "wow this\nis amaze");
  }

  /* TEST_CASE("mandelbrot") { */
  /*   test("mandelbrot.b", std::nullopt); */
  /* } */
}
