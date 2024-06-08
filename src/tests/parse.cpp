#include "pch.h"

#include "../brainfuck/file.h"
#include "../brainfuck/format.h"
#include "../brainfuck/parse.h"
#include "ApprovalTests.hpp"

namespace parse_tests {

void runSnapshotTest(const std::string filename) {
  const auto code = brainfuck::read(filename);
  ASSERT_TRUE(code.has_value());
  const auto instructions = brainfuck::parse(code.value());
  ApprovalTests::Approvals::verify(std::format("{}", instructions));
}

TEST(parse, empty_file) {
  runSnapshotTest("samples/tests/empty_file.b");
}

TEST(parse, echo) {
  runSnapshotTest("samples/echo.b");
}

}  // namespace parse_tests
