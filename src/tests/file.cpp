#include "pch.h"

#include "../brainfuck/file.h"
#include "ApprovalTests.hpp"

namespace file_tests {

void runSnapshotTest(std::string filename) {
  const auto content = brainfuck::read(filename);
  ASSERT_TRUE(content.has_value());
  ApprovalTests::Approvals::verify(std::string(content->begin(), content->end()));
}

TEST(file, empty) {
  runSnapshotTest("samples/tests/empty_file.b");
}

TEST(file, mandelbrot) {
  runSnapshotTest("samples/mandelbrot.b");
}

}  // namespace file_tests
