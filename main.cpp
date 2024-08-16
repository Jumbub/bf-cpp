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
  auto& dataAtRelativeOffset(Index relativeOffset) {
    return chunks[static_cast<uint64_t>(position + relativeOffset) / CHUNK_SIZE]
                 [static_cast<uint64_t>(position + relativeOffset) % CHUNK_SIZE];
  };
  auto& dataAtCurrentPosition() { return dataAtRelativeOffset(0); }
  void relativeMove(Index relativeOffset) { position += relativeOffset; }
  Index getAbsolutePosition() const { return position; };
};

struct TrackerIn {
  std::map<Tape::Index, char> input;

  bool operator==(const TrackerIn& rhs) const {
    return std::equal(this->input.begin(), this->input.end(), rhs.input.begin(), rhs.input.end());
  }
};

struct TrackerOut {
  std::map<Tape::Index, char> output;
  std::vector<char> print;
  int64_t moved;

  bool operator==(const TrackerOut& rhs) const {
    return this->moved == rhs.moved &&
           std::equal(this->print.begin(), this->print.end(), rhs.print.begin(), rhs.print.end()) &&
           std::equal(this->output.begin(), this->output.end(), rhs.output.begin(), rhs.output.end());
  }
};

struct Tracker : public TrackerIn, public TrackerOut {
  Tape::Index startAbsolutePosition;

  Tracker() = delete;
  Tracker(auto startAbsolutePosition) : startAbsolutePosition(startAbsolutePosition) {};
};

template <>
struct std::hash<TrackerIn> {
  std::size_t operator()(const TrackerIn& s) const noexcept { return s.input.size(); }
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

  for (size_t i = 0; i < nextLoopId; i++) {
    for (const auto& [hash, loopId] : loopIdForHash) {
      if (loopId == i) {
        std::cout << loopId << " " << hash << "\n";
      }
    }
  }
  std::cout << "\n\n\n";

  std::stack<Tracker> trackers;

  size_t codeIndex = 0;
  Tape tape;

  const auto relativePosition = [&]() { return tape.getAbsolutePosition() - trackers.top().startAbsolutePosition; };

  const auto read = [&]() -> auto& {
    auto& value = tape.dataAtCurrentPosition();

    if (!trackers.top().input.contains(relativePosition())) {
      trackers.top().input[relativePosition()] = value;
    }

    return value;
  };

  const auto increment = [&](char increment) {
    read() += increment;

    trackers.top().output[relativePosition()] = tape.dataAtCurrentPosition();
  };

  const auto move = [&](int64_t relativeOffset) {
    tape.relativeMove(relativeOffset);

    trackers.top().moved += relativeOffset;
  };

  const auto print = [&]() {
    const auto value = read();
    std::cout << value;

    trackers.top().print.push_back(value);
  };

  const auto execute = [&](const TrackerOut& output) {
    for (const auto& [relativeIndex, value] : output.output) {
      tape.dataAtRelativeOffset(relativeIndex) = value;
    }
    for (const char value : output.print) {
      std::cout << value;
    }
    tape.relativeMove(output.moved);
  };

  std::vector<std::unordered_map<TrackerIn, TrackerOut>> solvesForLoopId(nextLoopId);

  const auto ddd = [](TrackerIn in, TrackerOut out) {
    std::cout << "input: ";
    for (const auto& [relativeOffset, value] : in.input) {
      std::cout << "[" << (int)relativeOffset << "]=" << (int)value;
    }
    std::cout << "\noutput: ";
    for (const auto& [relativeOffset, value] : out.output) {
      std::cout << "[" << (int)relativeOffset << "]=" << (int)value;
    }
    std::cout << "\nprint: ";
    for (const auto value : out.print) {
      std::cout << (int)value;
    }
    std::cout << "\nmoved: " << out.moved;
    std::cout << "\n\n";
  };

  const auto dd = [&]() {
    for (size_t loopId = 0; loopId < solvesForLoopId.size(); loopId++) {
      std::cout << "(" << loopId << ")\n\n" << "";
      for (const auto& [in, out] : solvesForLoopId.at(loopId)) {
        ddd(in, out);
      }
    }
    std::cout << std::endl;
  };

  const auto checkIfSolved = [&](const size_t loopId) -> std::optional<TrackerOut> {
    // std::cout << "checking!" << loopId << " " << solvesForLoopId.at(loopId).size() << std::endl;
    for (const auto& [in, out] : solvesForLoopId.at(loopId)) {
      // ddd(in, out);
      const auto matches = std::all_of(in.input.cbegin(), in.input.cend(), [&](auto item) {
        const auto& [relativeOffset, value] = item;
        // std::cout << "SPAM" << std::endl;
        // std::cout << (int)relativeOffset << " " << (int)value << std::endl;
        // std::cout << (int)tape.dataAtRelativeOffset(relativeOffset) << std::endl;
        return tape.dataAtRelativeOffset(relativeOffset) == value;
      });
      if (matches) {
        // for (const auto& [relativeOffset, value] : solve.input) {
        //   std::cout << relativeOffset << " " << (int)value << " " << (int)tape.readRelativeOffset(relativeOffset)
        //             << std::endl;
        // }
        return out;
      }
    }
    return std::nullopt;
  };

  const auto storeSolvedTracker = [&]() {
    const auto tracker = trackers.top();

    const auto loopId = loopIdForStartIndex.at(startCodeIndexForEndCodeIndex.at(codeIndex));
    if (solvesForLoopId.at(loopId).contains(tracker)) {
      throw std::runtime_error("what the heck");
    }

    // std::cout << "stored!" << loopId << std::endl;
    // ddd(tracker, tracker);

    solvesForLoopId.at(loopId)[tracker] = tracker;

    trackers.pop();
    auto& parentTracker = trackers.top();
    for (const auto& [key, value] : tracker.input) {
      if (!parentTracker.input.contains(key)) {
        parentTracker.input[key] = value;
      }
    }
    for (const auto& [key, value] : tracker.output) {
      parentTracker.output[key] = value;
    }
    for (const auto value : tracker.print) {
      parentTracker.print.push_back(value);
    }
    parentTracker.moved += tracker.moved;
  };

  trackers.emplace(tape.getAbsolutePosition());
  while (codeIndex < code.size()) {
    // std::cout << codeIndex << std::endl;
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
          // std::cout << "SOLVED ALREADY!" << std::endl;
          execute(solved.value());
          codeIndex = endCodeIndexForStartCodeIndex.at(codeIndex) + 1;
          continue;
        }

        trackers.emplace(tape.getAbsolutePosition());
      } break;
      case ']': {
        if (read() != 0) {
          // restart loop
          codeIndex = startCodeIndexForEndCodeIndex.at(codeIndex) + 1;
          continue;
        }

        storeSolvedTracker();
      } break;
      case '+':
        increment(1);
        break;
      case '-':
        increment(-1);
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

  // std::cout << "\n\n";

  // dd();
};
