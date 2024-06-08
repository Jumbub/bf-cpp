#include "../brainfuck/file.h"
#include "doctest/doctest.h"
#include "utils.h"

void test(std::string relativePath) {
  const auto content = brainfuck::read("samples/" + relativePath);
  REQUIRE(content.has_value());
  requireSnapshot("file/" + relativePath, std::string(content->begin(), content->end()));
}

TEST_SUITE("file" * doctest::timeout(10)) {
  TEST_CASE("empty") {
    test("tests/empty_file.b");
  }

  TEST_CASE("mandelbrot") {
    test("mandelbrot.b");
  }
}
