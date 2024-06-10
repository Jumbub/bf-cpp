#include "../brainfuck/go.h"
#include "doctest/doctest.h"
#include "utils.h"

using namespace brainfuck;

void test(
    const std::string filename,
    const std::optional<std::string> input = std::nullopt,
    const Error expectedError = Error::NONE) {
  const auto stopCapturingIO = startCapturingIO(input);
  CAPTURE(filename);

  try {
    const auto error = go("samples/" + filename);

    const auto output = stopCapturingIO();
    CHECK_MESSAGE(error == expectedError, "Unexpected error code: ", error);
    REQUIRE_SNAPSHOT("go/" + filename, output);
  } catch (...) {
    stopCapturingIO();
    throw;
  }
}

TEST_CASE("go") {
  const auto error = go("this-file-does-not-exist");
  REQUIRE(error == Error::PROGRAM_NOT_FOUND);

  test("tests/empty_file.b");
  test("tests/no_loop_hello.b");
  test("tests/unmatched_brace_[.b", std::nullopt, Error::UNMATCHED_BRACE);
  test("tests/unmatched_brace_].b", std::nullopt, Error::UNMATCHED_BRACE);
  test("tests/loop_til_zero.b");
  test("hello_world.b");
  test("dbfi.b", "--[>--->->->++>-<<<<<-------]>--.>---------.>--..+++.>----.>+++++++++.<<.+++.------.<-.>>+.!");
  test("rot13.b", "~mlk zyx");
  test("echo.b", "wow this\nis amaze");
  test("numwarp.b", "()-./0123456789abcdef()-./0123456789abcdef");
  test("392quine.b");
  test("bitwidth.b");
  const auto glider =
      "ac\nbc\ncc\ncb\nba\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
      "\n\n\n\n\n\n\n\n\nq\n";
  test("life.b", glider);
  /* test("mandelbrot.b"); */
}
