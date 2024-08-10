#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <stack>
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

  size_t nextLoopId = 0;
  std::unordered_map<std::string, size_t> loopIdForHash;
  std::unordered_map<size_t, size_t> loopIdForCodeIndex;
  std::unordered_map<size_t, size_t> endCodeIndexForStartCodeIndex;
  std::unordered_map<size_t, size_t> startCodeIndexForEndCodeIndex;
  {
    std::stack<std::string> hashes;
    std::stack<size_t> startCodeIndexes;
    std::stack<size_t> loopIds;

    {  // app "loop"
      hashes.push("");
    }

    for (size_t codeIndex = 0; codeIndex < code.size(); codeIndex++) {
      switch (code[codeIndex]) {
        case '[': {
          hashes.push("");
          startCodeIndexes.push(codeIndex);
          loopIds.push(nextLoopId);
        } break;
        case ']': {
          startCodeIndexForEndCodeIndex[codeIndex] = startCodeIndexes.top();
          endCodeIndexForStartCodeIndex[startCodeIndexes.top()] = codeIndex;

          const auto hash = hashes.top();
          std::optional<size_t> currentLoopId = std::nullopt;  // unitialized value
          if (loopIdForHash.contains(hash)) {
            loopIdForCodeIndex[startCodeIndexes.top()] = loopIdForHash.at(hash);
            currentLoopId.emplace(loopIdForHash.at(hash));
          } else {
            loopIdForHash[hash] = loopIds.top();
            loopIdForCodeIndex[startCodeIndexes.top()] = loopIds.top();
            nextLoopId += 1;
          }
          hashes.pop();
          hashes.top() += "[" + std::to_string(loopIds.top()) + "]";

          startCodeIndexes.pop();
          loopIds.pop();
        } break;
        case '+':
        case '-':
        case '>':
        case '<':
        case '.':
          hashes.top().push_back(code[codeIndex]);
          break;
      }
    }

    {  // app "loop"
      loopIdForHash[hashes.top()] = nextLoopId;
      loopIdForCodeIndex[0] = nextLoopId;
      nextLoopId += 1;
    }
  }

  std::cout << "start:\n\n\n";
  for (size_t i = 0; i < nextLoopId; i++) {
    for (const auto& [hash, loopId] : loopIdForHash) {
      if (loopId == i) {
        std::cout << loopId << " " << hash << "\n";
      }
    }
  }
  std::cout << nextLoopId << "\n";
  std::cout << "\n\n\n";

  size_t codeIndex = 0;
  InfiniteTape tape;
  while (codeIndex < code.size()) {
    switch (code[codeIndex]) {
      case '[':
        if (tape.get() == 0) {
          codeIndex = endCodeIndexForStartCodeIndex[codeIndex];
        }
        break;
      case ']':
        if (tape.get() != 0) {
          codeIndex = startCodeIndexForEndCodeIndex[codeIndex];
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
    }
    codeIndex += 1;
  }
};
