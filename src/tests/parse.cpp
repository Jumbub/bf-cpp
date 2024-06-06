#include "../brainfuck/file.h"
#include "../brainfuck/format.h"
#include "../brainfuck/parse.h"
#include "ApprovalTests.hpp"
#include "gtest/gtest.h"

namespace parse_tests {

void runSnapshotTest(const std::string filename) {
  const auto code = brainfuck::read(filename);
  ASSERT_TRUE(code.has_value());
  const auto instructions = brainfuck::parse(code.value());

  std::string humanReadableInstructions;
  for (const auto instruction : instructions) {
    humanReadableInstructions += std::format("{}", instruction);
  }

  ApprovalTests::Approvals::verify(humanReadableInstructions);
}

TEST(parse, empty_file) {
  runSnapshotTest("samples/tests/empty_file.b");
}

}  // namespace parse_tests
