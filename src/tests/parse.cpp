#include <format>
#include "../brainfuck/file.h"
#include "../brainfuck/format.h"
#include "../brainfuck/parse.h"
#include "doctest/doctest.h"
#include "utils.h"

using namespace brainfuck;

namespace parse_suite {

void test(const std::string filename) {
  CAPTURE(filename);

  const auto code = read("samples/" + filename);
  REQUIRE(code.has_value());
  const auto instructions = parse(code.value());
  REQUIRE_MESSAGE((bool)instructions, "Failed to parse");
  std::string output;
  for (size_t instructionPointer = 0; instructionPointer < instructions->size(); instructionPointer++) {
    const auto instruction = instructions.value()[instructionPointer];
    output += formatInstruction(instruction) + "\n";
  }
  if (output.empty()) {
    output = "<empty bytecode>";
  }
  REQUIRE_SNAPSHOT("parse/" + filename, output);
}

TEST_CASE("parse") {
  test("tests/empty_file.b");
  test("tests/strip_non_code.b");
  test("tests/no_loop_hello.b");
  test("tests/loop_til_zero.b");
  test("tests/move_til_zero.b");
  test("tests/move_til_zero_twice.b");
  test("tests/transfer_value.b");
  test("tests/transfer_value_with_extra.b");
  test("tests/transfer_value_far.b");
  test("tests/transfer_value_looping.b");
  test("tests/multiply_value.b");
  test("tests/multiply_value_with_extra.b");
  test("tests/complex_multiply_value.b");
  test("tests/complex_multiply_value_multi_output.b");
  test("tests/complex_multiply_value_multi_output_spread.b");
  test("tests/sort_loop_offsets.b");
  test("hello_world.b");
  test("dbfi.b");
  test("rot13.b");
  test("echo.b");
  test("numwarp.b");
  test("392quine.b");
  test("bitwidth.b");
  test("life.b");
  test("mandelbrot.b");
}

}  // namespace parse_suite
