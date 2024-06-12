#include <format>
#include "../brainfuck/file.h"
#include "../brainfuck/parse.h"
#include "doctest/doctest.h"
#include "utils.h"

using namespace brainfuck;

void parseTest(const std::string filename) {
  CAPTURE(filename);

  const auto code = read("samples/" + filename);
  REQUIRE(code.has_value());
  const auto instructions = parse(code.value());
  REQUIRE_MESSAGE((bool)instructions, "Failed to parse, error code: ", instructions.error());
  std::string output;
  for (const auto instruction : *instructions) {
    output += std::format("{} {}\n", (char)instruction.type, instruction.value);
  }
  if (output.empty()) {
    output = "<empty bytecode>";
  }
  REQUIRE_SNAPSHOT("parse/" + filename, output);
}

TEST_CASE("parse") {
  parseTest("tests/empty_file.b");
  parseTest("tests/no_loop_hello.b");
  parseTest("tests/loop_til_zero.b");
  parseTest("hello_world.b");
  parseTest("dbfi.b");
  parseTest("rot13.b");
  parseTest("echo.b");
  parseTest("numwarp.b");
  parseTest("392quine.b");
  parseTest("bitwidth.b");
  parseTest("life.b");
  parseTest("mandelbrot.b");
}
