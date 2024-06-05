#include <iostream>
#include "../brainfuck/file.h"
#include "ApprovalTests.hpp"
#include "gtest/gtest.h"

TEST(read, empty) {
  const auto content = brainfuck::read("src/programs/empty.b");
  ASSERT_TRUE(content.has_value());
  ApprovalTests::Approvals::verify(std::string(content->begin(), content->end()));
}

TEST(read, mandelbrot) {
  const auto content = brainfuck::read("src/programs/mandelbrot.b");
  ASSERT_TRUE(content.has_value());
  ApprovalTests::Approvals::verify(std::string(content->begin(), content->end()));
}

TEST(read, hello_world) {
  const auto content = brainfuck::read("src/programs/hello-world.b");
  ASSERT_TRUE(content.has_value());
  ApprovalTests::Approvals::verify(std::string(content->begin(), content->end()));
}
