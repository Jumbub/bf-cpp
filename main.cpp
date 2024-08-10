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
  std::pair<int64_t, int64_t> index() { return {tapeIndex, cellIndex}; };
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
  std::unordered_map<size_t, size_t> loopIdForStartIndex;
  std::unordered_map<size_t, size_t> endCodeIndexForStartCodeIndex;
  std::unordered_map<size_t, size_t> startCodeIndexForEndCodeIndex;
  {
    std::stack<std::string> hashes;
    std::stack<size_t> startCodeIndexes;

    {  // app "loop"
      hashes.push("");
    }

    for (size_t codeIndex = 0; codeIndex < code.size(); codeIndex++) {
      switch (code[codeIndex]) {
        case '[': {
          hashes.push("");
          startCodeIndexes.push(codeIndex);
        } break;
        case ']': {
          const auto startCodeIndex = startCodeIndexes.top();
          startCodeIndexes.pop();

          startCodeIndexForEndCodeIndex[codeIndex] = startCodeIndex;
          endCodeIndexForStartCodeIndex[startCodeIndex] = codeIndex;

          const auto hash = hashes.top();
          size_t currentLoopId;
          if (loopIdForHash.contains(hash)) {
            const auto loopId = loopIdForHash.at(hash);

            loopIdForStartIndex[startCodeIndex] = loopId;
            currentLoopId = loopId;
          } else {
            const auto loopId = nextLoopId;
            nextLoopId += 1;

            loopIdForHash[hash] = loopId;
            loopIdForStartIndex[startCodeIndex] = loopId;
            currentLoopId = loopId;
          }

          hashes.pop();
          hashes.top() += "[" + std::to_string(currentLoopId) + "]";
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
      loopIdForStartIndex[nextLoopId] = nextLoopId;
      nextLoopId += 1;
    }
  }

  std::unordered_map<size_t, size_t> executionsOfLoop;

  std::map<std::pair<int64_t, int64_t>, char> input;
  std::map<std::pair<int64_t, int64_t>, char> output;
  std::vector<char> print;

  size_t codeIndex = 0;
  InfiniteTape tape;
  int i = 0;

  const auto read = [&]() {
    auto& value = tape.get();
    const auto tapeIndex = tape.index();
    if (!input.contains(tapeIndex)) {
      input[tapeIndex] = value;
    }
    return value;
  };

  const auto increment = [&](char increment) {
    auto& value = tape.get();
    value += increment;
    output.insert_or_assign(tape.index(), value);
  };

  while (codeIndex < code.size()) {
    if (i++ > 500000000) {
      break;
    }
    switch (code[codeIndex]) {
      case '[':
        if (read() == 0) {
          codeIndex = endCodeIndexForStartCodeIndex[codeIndex];
        } else {
          const auto loopId = loopIdForStartIndex.at(codeIndex);
          executionsOfLoop[loopId] += 1;
        }
        break;
      case ']':
        if (read() != 0) {
          codeIndex = startCodeIndexForEndCodeIndex[codeIndex];
        }
        break;
      case '+':
        increment(1);
        break;
      case '-':
        increment(-1);
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

  std::cout << "start:\n\n\n";
  for (size_t i = 0; i < nextLoopId; i++) {
    if (!executionsOfLoop.contains(i)) {
      continue;
    }
    for (const auto& [hash, loopId] : loopIdForHash) {
      if (loopId == i) {
        std::cout << loopId << " " << hash << "\n";
      }
    }
    for (const auto& [startIndex, loopId] : loopIdForStartIndex) {
      if (loopId == i) {
        std::cout << startIndex << ",";
      }
    }
    std::cout << "\nexecutions: " << executionsOfLoop[i] << "\n";
  }
  std::cout << nextLoopId << "\n";
  std::cout << "\n\n\n";
};
