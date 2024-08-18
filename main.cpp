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
  using Data = char;
  using Cell = std::pair<Index, Data>;

 private:
  static constexpr uint64_t CHUNK_SIZE = 4096;
  std::unordered_map<uint64_t, Data[CHUNK_SIZE]> chunks;
  Index position = 0;

 public:
  Data& relativeData(Index relativeOffset) {
    return chunks[static_cast<uint64_t>(position + relativeOffset) / CHUNK_SIZE]
                 [static_cast<uint64_t>(position + relativeOffset) % CHUNK_SIZE];
  };
  void relativeMove(Index relativeOffset) { position += relativeOffset; }
};

struct LoopInput {
  std::vector<std::pair<Tape::Index, char>> input;

  bool operator==(const LoopInput& rhs) const {
    return std::equal(this->input.begin(), this->input.end(), rhs.input.begin(), rhs.input.end());
  }
};

struct LoopOutput {
  std::map<Tape::Index, char> output;
  std::vector<char> print;
  int64_t moved;

  bool operator==(const LoopOutput& rhs) const {
    return this->moved == rhs.moved &&
           std::equal(this->print.begin(), this->print.end(), rhs.print.begin(), rhs.print.end()) &&
           std::equal(this->output.begin(), this->output.end(), rhs.output.begin(), rhs.output.end());
  }
};

struct Hash : public LoopInput, public LoopOutput {};

template <>
struct std::hash<LoopInput> {
  std::size_t operator()(const LoopInput& s) const noexcept { return s.input.size(); }
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
    auto& data = tape.relativeData(0);

    auto& wipHash = wipHashes.top();
    if (std::all_of(wipHash.input.cbegin(), wipHash.input.cend(), [&](Tape::Cell cell) {
          return cell.first != wipHash.moved;
        })) {
      wipHashes.top().input.push_back({wipHash.moved, data});
    }

    return data;
  };

  const auto add = [&](char increment) {
    auto& data = read();
    data += increment;

    auto& wipHash = wipHashes.top();
    wipHash.output[wipHash.moved] = data;
  };

  const auto move = [&](int64_t increment) {
    tape.relativeMove(increment);

    auto& wipHash = wipHashes.top();
    wipHash.moved += increment;
  };

  const auto print = [&]() {
    const auto data = read();
    std::cout << data << std::flush;  // todo remove when faster

    auto& wipHash = wipHashes.top();
    wipHash.print.push_back(data);
  };

  const auto execute = [&](const LoopInput& query, const LoopOutput& solution) {
    auto& wipHash = wipHashes.top();

    for (const char data : solution.print) {
      std::cout << data << std::flush;  // todo remove when faster
      wipHash.print.push_back(data);
    }

    for (const auto& [relativeOffset, data] : query.input) {
      if (std::all_of(wipHash.input.cbegin(), wipHash.input.cend(), [&](Tape::Cell cell) {
            return cell.first != wipHash.moved + relativeOffset;
          })) {
        wipHash.input.push_back({wipHash.moved + relativeOffset, data});
      }
    }

    for (const auto& [relativeOffset, data] : solution.output) {
      tape.relativeData(relativeOffset) = data;
      wipHash.output[wipHash.moved + relativeOffset] = data;
    }

    tape.relativeMove(solution.moved);
    wipHash.moved += solution.moved;
  };

  std::vector<std::unordered_map<LoopInput, LoopOutput>> solvesForLoopId(nextLoopId);

  const auto checkIfSolved = [&](const size_t loopId) -> std::optional<std::pair<LoopInput, LoopOutput>> {
    for (const auto& [in, out] : solvesForLoopId.at(loopId)) {
      const auto matches = std::all_of(in.input.cbegin(), in.input.cend(), [&](auto item) {
        const auto& [relativeOffset, data] = item;
        return tape.relativeData(relativeOffset) == data;
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

    for (const auto& [relativeOffset, data] : solvedHash.input) {
      if (std::all_of(wipHash.input.cbegin(), wipHash.input.cend(), [&](Tape::Cell cell) {
            return cell.first != wipHash.moved + relativeOffset;
          })) {
        wipHash.input.push_back({wipHash.moved + relativeOffset, data});
      }
    }

    for (const auto& [relativeOffset, data] : solvedHash.output) {
      wipHash.output[wipHash.moved + relativeOffset] = data;
    }

    for (const auto data : solvedHash.print) {
      wipHash.print.push_back(data);
    }

    wipHash.moved += solvedHash.moved;
  };

  wipHashes.emplace();
  int i = 0;
  while (codeIndex < code.size()) {
    if (i++ > 10000000) {
      break;
    }

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
