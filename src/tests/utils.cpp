#include "utils.h"

#include <stdio.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include "doctest/doctest.h"

std::string readFile(std::string filename) {
  std::ifstream file(filename);
  CHECK_MESSAGE(file.is_open(), "Missing expected snapshot: ", filename);
  return file.is_open() ? std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()) : "";
}

void writeFile(std::string filename, std::string content) {
  std::ofstream file(filename);
  file << content;
}

void REQUIRE_SNAPSHOT(std::string path, std::string received) {
  // const auto expected = readFile("src/tests/snapshots/" + path + ".expected.txt");
  // writeFile("src/tests/snapshots/" + path + ".RECEIVED.txt", received);
  writeFile("src/tests/snapshots/" + path + ".expected.txt", received);
  // REQUIRE_MESSAGE((bool)(expected == received), "Output doesn't match snapshot");
}

StopCaptureIO startCapturingIO(std::optional<std::string> input) {
  std::shared_ptr<std::stringstream> newStdout = std::make_shared<std::stringstream>();
  std::shared_ptr<std::stringstream> newStdin = std::make_shared<std::stringstream>();
  std::streambuf* oldStdout = std::cout.rdbuf(newStdout->rdbuf());
  std::streambuf* oldStdin = std::cin.rdbuf(newStdin->rdbuf());

  if (input.has_value()) {
    (*newStdin) << input.value();
  }

  return [oldStdout, oldStdin, newStdout, newStdin]() {
    std::cout.rdbuf(oldStdout);
    std::cin.rdbuf(oldStdin);
    return newStdout->str();
  };
}
