#define APPROVALS_GOOGLETEST  // This tells Approval Tests to provide a main() - only do this in one cpp file
#include "ApprovalTests.hpp"

#include "../brainfuck/file.h"
#include "../brainfuck/go.h"
#include "../brainfuck/parse.h"

auto directoryDisposer = ApprovalTests::Approvals::useApprovalsSubdirectory("snapshots");

namespace brainfuck {

std::ostream& operator<<(std::ostream& os, const brainfuck::Instruction& instruction) {
  os << "[type: " << (char)instruction.type << std::string(" value: ") << (int)instruction.value << "]";
  return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<brainfuck::Instruction>& instructions) {
  for (int i = 0; i < instructions.size(); i++) {
    os << instructions[i];
  }
  return os;
}
}  // namespace brainfuck

TEST(GoogleTestStarter, Sample) {
  const std::vector<char> x = {'+'};
  const auto output = brainfuck::parse(x);
  ApprovalTests::Approvals::verify(output);
}

/* std::string executeFile(const char* filename) { */
/*   testing::internal::CaptureStdout(); */
/*   brainfuck::go(filename); */
/*   return testing::internal::GetCapturedStdout(); */
/* } */

/* std::string readFile(const char* filename) { */
/*   const auto expectVector = file::read(filename); */
/*   if (!expectVector.has_value()) { */
/*     std::cerr << "missing test file" << std::endl; */
/*   } */
/*   return {expectVector.value().begin(), expectVector.value().end()}; */
/* } */

TEST(file, read) {
  /* (file::read("src/tests/programs/hello_world.b"), readFile("src/tests/programs/hello_world.txt")); */
}

/* TEST(programs, empty) { */
/*   EXPECT_EQ(executeFile("src/tests/programs/empty.b"), readFile("src/tests/programs/empty.txt")); */
/* } */

/* TEST(programs, hello_world) { */
/*   EXPECT_EQ(executeFile("src/tests/programs/hello_world.b"), readFile("src/tests/programs/hello_world.txt")); */
/* } */

/* TEST(programs, mandelbrot) { */
/*   EXPECT_EQ(executeFile("src/tests/programs/mandelbrot.b"), readFile("src/tests/programs/mandelbrot.b")); */
/* } */
