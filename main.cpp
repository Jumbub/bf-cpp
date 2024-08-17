#include <algorithm>
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

class Tape {
 public:
  using Index = int64_t;

 private:
  static constexpr uint64_t CHUNK_SIZE = 4096;
  std::unordered_map<uint64_t, char[CHUNK_SIZE]> chunks;
  Index position = 0;

 public:
  auto& relativeCell(Index relativeOffset) {
    return chunks[static_cast<uint64_t>(position + relativeOffset) / CHUNK_SIZE]
                 [static_cast<uint64_t>(position + relativeOffset) % CHUNK_SIZE];
  };
  void relativeMove(Index relativeOffset) { position += relativeOffset; }
};

struct HashQuery {
  std::map<Tape::Index, char> input;

  bool operator==(const HashQuery& rhs) const {
    return std::equal(this->input.begin(), this->input.end(), rhs.input.begin(), rhs.input.end());
  }
};

struct HashSolution {
  std::map<Tape::Index, char> output;
  std::vector<char> print;
  int64_t moved;

  bool operator==(const HashSolution& rhs) const {
    return this->moved == rhs.moved &&
           std::equal(this->print.begin(), this->print.end(), rhs.print.begin(), rhs.print.end()) &&
           std::equal(this->output.begin(), this->output.end(), rhs.output.begin(), rhs.output.end());
  }
};

struct Hash : public HashQuery, public HashSolution {};

template <>
struct std::hash<HashQuery> {
  std::size_t operator()(const HashQuery& s) const noexcept { return s.input.size(); }
};

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Usage: ./brainfuck <program.b>\n";
    return 1;
  }

  const auto maybeCode = load(argv[1]);
  if (!maybeCode) {
    std::cerr << "Error: unable to read '" << argv[1] << "'\n";
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
          size_t loopId = 1337;
          if (loopIdForHash.contains(hash)) {
            loopId = loopIdForHash.at(hash);
            loopIdForStartIndex[startCodeIndex] = loopId;
          } else {
            loopId = nextLoopId;
            nextLoopId += 1;
            loopIdForHash[hash] = loopId;
            loopIdForStartIndex[startCodeIndex] = loopId;
          }

          hashes.pop();
          hashes.top() += "[" + std::to_string(loopId) + "]";
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
      loopIdForStartIndex[0] = nextLoopId;
      nextLoopId += 1;
    }
  }

  std::stack<Hash> wipHashes;

  size_t codeIndex = 0;
  Tape tape;

  const auto read = [&]() -> auto& {
    auto& cell = tape.relativeCell(0);

    auto& wipHash = wipHashes.top();
    if (!wipHash.input.contains(wipHash.moved)) {
      wipHashes.top().input[wipHash.moved] = cell;
    }

    return cell;
  };

  const auto add = [&](char increment) {
    auto& cell = read();
    cell += increment;

    auto& wipHash = wipHashes.top();
    wipHash.output[wipHash.moved] = cell;
  };

  const auto move = [&](int64_t increment) {
    tape.relativeMove(increment);

    auto& wipHash = wipHashes.top();
    wipHash.moved += increment;
  };

  const auto print = [&]() {
    const auto value = read();
    std::cout << value;

    auto& wipHash = wipHashes.top();
    wipHash.print.push_back(value);
  };

  const auto execute = [&](const HashQuery& query, const HashSolution& solution) {
    auto& wipHash = wipHashes.top();

    for (const char value : solution.print) {
      std::cout << value;
      wipHash.print.push_back(value);
    }

    for (const auto& [relativeOffset, value] : query.input) {
      if (!wipHash.input.contains(wipHash.moved + relativeOffset)) {
        wipHash.input[wipHash.moved + relativeOffset] = value;
      }
    }

    for (const auto& [relativeOffset, value] : solution.output) {
      tape.relativeCell(relativeOffset) = value;
      wipHash.output[wipHash.moved + relativeOffset] = value;
    }

    tape.relativeMove(solution.moved);
    wipHash.moved += solution.moved;
  };

  std::vector<std::unordered_map<HashQuery, HashSolution>> solvesForLoopId(nextLoopId);

  const auto checkIfSolved = [&](const size_t loopId) -> std::optional<std::pair<HashQuery, HashSolution>> {
    for (const auto& [in, out] : solvesForLoopId.at(loopId)) {
      const auto matches = std::all_of(in.input.cbegin(), in.input.cend(), [&](auto item) {
        const auto& [relativeOffset, value] = item;
        return tape.relativeCell(relativeOffset) == value;
      });
      if (matches) {
        return std::pair{in, out};
      }
    }
    return std::nullopt;
  };

  const auto storeSolution = [&]() {
    const Hash solvedHash = wipHashes.top();

    const auto loopId = loopIdForStartIndex.at(startCodeIndexForEndCodeIndex.at(codeIndex));
    if (solvesForLoopId.at(loopId).contains(solvedHash)) {
      throw std::runtime_error("what the heck");
    }

    solvesForLoopId.at(loopId)[solvedHash] = solvedHash;

    wipHashes.pop();
    auto& wipHash = wipHashes.top();

    for (const auto& [relativeOffset, value] : solvedHash.input) {
      if (!wipHash.input.contains(wipHash.moved + relativeOffset)) {
        wipHash.input[wipHash.moved + relativeOffset] = value;
      }
    }

    for (const auto& [relativeOffset, value] : solvedHash.output) {
      wipHash.output[wipHash.moved + relativeOffset] = value;
    }

    for (const auto value : solvedHash.print) {
      wipHash.print.push_back(value);
    }

    wipHash.moved += solvedHash.moved;
  };

  wipHashes.emplace();
  while (codeIndex < code.size()) {
    switch (code[codeIndex]) {
      case '[': {
        if (read() == 0) {
          // skip loop
          codeIndex = endCodeIndexForStartCodeIndex.at(codeIndex) + 1;
          continue;
        }

        const auto loopId = loopIdForStartIndex.at(codeIndex);

        const auto solved = checkIfSolved(loopId);
        if (solved.has_value()) {
          execute(solved.value().first, solved.value().second);
          codeIndex = endCodeIndexForStartCodeIndex.at(codeIndex) + 1;
          continue;
        }

        wipHashes.emplace();
      } break;
      case ']': {
        if (read() != 0) {
          // restart loop
          codeIndex = startCodeIndexForEndCodeIndex.at(codeIndex) + 1;
          continue;
        }

        storeSolution();
      } break;
      case '+':
        add(1);
        break;
      case '-':
        add(-1);
        break;
      case '>':
        move(1);
        break;
      case '<':
        move(-1);
        break;
      case '.':
        print();
        break;
    }
    codeIndex += 1;
  }
};
