/* #include <iostream> */
/* #include "../brainfuck/go.h" */
/* #include "ApprovalTests.hpp" */
/* #include "gtest/gtest.h" */

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

/* TEST(file, read) { */
/*   /1* (file::read("src/tests/programs/hello_world.b"), readFile("src/tests/programs/hello_world.txt")); *1/ */
/* } */

/* TEST(programs, empty) { */
/*   EXPECT_EQ(brainfuck::("src/tests/programs/empty.b"), readFile("src/tests/programs/empty.txt")); */
/* } */

/* TEST(programs, hello_world) { */
/*   EXPECT_EQ(executeFile("src/tests/programs/hello_world.b"), readFile("src/tests/programs/hello_world.txt")); */
/* } */

/* TEST(programs, mandelbrot) { */
/*   EXPECT_EQ(executeFile("src/tests/programs/mandelbrot.b"), readFile("src/tests/programs/mandelbrot.b")); */
/* } */
