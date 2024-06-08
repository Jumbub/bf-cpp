#include "utils.h"

#include <stdio.h>
#include <unistd.h>
#include <sstream>
#include "../brainfuck/go.h"
#include "doctest/doctest.h"

namespace go_tests {

void runSnapshotTest(const std::string filename) {
  /* testing::internal::CaptureStdout(); */

  std::stringstream buffer;
  // Redirect std::cout to buffer
  std::streambuf* prevcoutbuf = std::cout.rdbuf(buffer.rdbuf());

  const auto result = brainfuck::go("samples/" + filename);

  std::string text = buffer.str();

  // Restore original buffer before exiting
  std::cout.rdbuf(prevcoutbuf);

  requireSnapshot("go/" + filename, text);

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

TEST_CASE("empty_file") {
  runSnapshotTest("tests/empty_file.b");
}

TEST_CASE("no_loop_hello") {
  runSnapshotTest("tests/no_loop_hello.b");
}

TEST_CASE("echo") {
  injectStdin("wow this\nis amaze");
  runSnapshotTest("echo.b");
}

}  // namespace go_tests
