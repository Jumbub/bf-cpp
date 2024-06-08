#include "../brainfuck/file.h"
#include "doctest/doctest.h"
#include "utils.h"

void test(std::string relativePath) {
  const auto content = brainfuck::read("samples/" + relativePath);
  CHECK_MESSAGE(content.has_value(), "Failed to read file: samples/", relativePath);

  const auto contentOrEmptyString = content.has_value() ? std::string(content->begin(), content->end()) : "";
  REQUIRE_SNAPSHOT("file/" + relativePath, contentOrEmptyString);
}

TEST_SUITE("file" * doctest::timeout(10)) {
  TEST_CASE("empty") {
    test("tests/empty_file.b");
  }

  TEST_CASE("mandelbrot") {
    test("mandelbrot.b");
  }
}
