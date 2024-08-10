#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

std::optional<std::vector<char>> load(const std::string filename) {
  std::ifstream file(filename, std::ios::binary);
  if (!file.good()) {
    return std::nullopt;
  }
  const auto size = std::filesystem::file_size(filename);
  std::vector<char> buffer(size);
  file.read(buffer.data(), static_cast<std::streamsize>(size));
  return buffer;
}

class InfiniteTape /* ~37 zettabytes in each direction */ {
  static constexpr int64_t TAPE_SIZE = 4096;
  std::unordered_map<int64_t, char[TAPE_SIZE]> tapes;
  int64_t tapeIndex = 0;
  int64_t cellIndex = 0;

 public:
  auto& get() { return tapes[tapeIndex][cellIndex]; }
  void move(auto move) {
    cellIndex += move;
    while (cellIndex < 0) {
      cellIndex += TAPE_SIZE;
      tapeIndex -= 1;
    }
    while (cellIndex >= TAPE_SIZE) {
      cellIndex -= TAPE_SIZE;
      tapeIndex += 1;
    }
  }
};

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cout << "Usage: ./brainfuck <program.b>\n";
    return 1;
  }

  const auto maybeCode = load(argv[1]);
  if (!maybeCode) {
    std::cout << "Error: unable to read '" << argv[1] << "'\n";
    return 2;
  }
  const auto code = *maybeCode;

  size_t codeIndex = 0;
  InfiniteTape tape;

  while (codeIndex < code.size()) {
    switch (code[codeIndex]) {
      case '[':
        if (tape.get() == 0) {
          auto depth = 0;
          codeIndex += 1;
          while (code[codeIndex] != ']' || depth > 0) {
            if (code[codeIndex] == '[') {
              depth += 1;
            } else if (code[codeIndex] == ']') {
              depth -= 1;
            }
            codeIndex += 1;
          }
        }
        break;
      case ']':
        if (tape.get() != 0) {
          auto depth = 0;
          codeIndex -= 1;
          while (code[codeIndex] != '[' || depth > 0) {
            if (code[codeIndex] == '[') {
              depth -= 1;
            } else if (code[codeIndex] == ']') {
              depth += 1;
            }
            codeIndex -= 1;
          }
        }
        break;
      case '+':
        tape.get() += 1;
        break;
      case '-':
        tape.get() -= 1;
        break;
      case '>':
        tape.move(1);
        break;
      case '<':
        tape.move(-1);
        break;
      case '.':
        std::cout << tape.get();
        break;
      case ',':
        std::cin >> tape.get();
        break;
    }
    codeIndex += 1;
  }
};
