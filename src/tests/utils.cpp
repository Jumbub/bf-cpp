#include "utils.h"

#include <fstream>
#include "doctest/doctest.h"

std::string readFile(std::string filename) {
  std::ifstream file(filename);
  REQUIRE(file.is_open());
  return {(std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()};
}

void writeFile(std::string filename, std::string content) {
  std::ofstream file(filename);
  file << content;
}

void requireSnapshot(std::string path, std::string received) {
  const auto expected = readFile("src/tests/snapshots/" + path + ".expected.txt");
  writeFile("src/tests/snapshots/" + path + ".RECEIVED.txt", received);
  REQUIRE((bool)(expected == received));
}
