#pragma once

#include "types.h"

namespace brainfuck {

struct Debug {
  Debug(const std::vector<Instruction>& instructions);

  void trackInstruction(const size_t index);
  void done() const;

  const std::vector<Instruction>& instructions;
  std::vector<uint64_t> executions;
  std::chrono::high_resolution_clock::time_point time;
  std::vector<std::chrono::duration<double>> times;
};

}  // namespace brainfuck
