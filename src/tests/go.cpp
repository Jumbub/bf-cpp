#include "../brainfuck/go.h"
#include "pch.h"

namespace go_tests {

void runSnapshotTest(const std::string filename) {
  /* testing::internal::CaptureStdout(); */
  /* const int result = brainfuck::go(filename); */
  /* ASSERT_EQ(result, 0); */
  /* ApprovalTests::Approvals::verify(testing::internal::GetCapturedStdout()); */
}

void injectStdin(const std::string data) {
  // https://stackoverflow.com/a/13081732
  int fd[2];
  pipe(fd);
  close(0);    // 0:stdin
  dup(fd[0]);  // make read pipe be stdin
  close(fd[0]);
  fd[0] = 0;
  write(fd[1], data.c_str(), data.size() + 1);  // write "some text" to stdin
}

TEST(go, empty_file) {
  runSnapshotTest("samples/tests/empty_file.b");
}

TEST(go, no_loop_hello) {
  runSnapshotTest("samples/tests/no_loop_hello.b");
}

TEST(go, echo) {
  injectStdin("wow this\nis amaze");
  runSnapshotTest("samples/echo.b");
}

}  // namespace go_tests
