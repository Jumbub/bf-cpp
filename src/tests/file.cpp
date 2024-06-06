#include <iostream>
#include "../brainfuck/file.h"
#include "ApprovalTests.hpp"
#include "gtest/gtest.h"

namespace file_tests {

void runSnapshotTest(std::string filename) {
  const auto content = brainfuck::read(filename);
  ASSERT_TRUE(content.has_value());
  ApprovalTests::Approvals::verify(std::string(content->begin(), content->end()));
}

TEST(read, empty) {
  runSnapshotTest("samples/tests/empty_file.b");
}

TEST(read, mandelbrot) {
  runSnapshotTest("samples/mandelbrot.b");
}

}  // namespace file_tests
