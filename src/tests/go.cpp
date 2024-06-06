#include "../brainfuck/go.h"
#include "ApprovalTests.hpp"
#include "gtest/gtest.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

namespace go_tests {

void runSnapshotTest(const std::string filename) {
  testing::internal::CaptureStdout();
  auto result = brainfuck::go(filename);
  ASSERT_EQ(result, 0);
  ApprovalTests::Approvals::verify(testing::internal::GetCapturedStdout());
}

TEST(go, empty_file) {
  runSnapshotTest("samples/tests/empty_file.b");
}

TEST(go, no_loop_hello) {
  runSnapshotTest("samples/tests/no_loop_hello.b");
}

TEST(go, echo) {
  int fd[2];
  pipe(fd);
  close(0);    // 0:stdin
  dup(fd[0]);  // make read pipe be stdin
  close(fd[0]);
  fd[0] = 0;

  write(fd[1], "wow this\nis amaze", 18);  // write "some text" to stdin

  runSnapshotTest("samples/echo.b");
}

}  // namespace go_tests
