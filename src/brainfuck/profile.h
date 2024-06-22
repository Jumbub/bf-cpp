#pragma once

#include <chrono>
#include "types.h"

namespace brainfuck {

struct Profile {
  Profile(const std::vector<Instruction>& instructions);
  ~Profile();

  void instruction(const Instruction*);

 private:
  const std::vector<Instruction>& instructions;
  std::vector<uint64_t> executions;
  std::chrono::high_resolution_clock::time_point time;
  std::vector<std::chrono::duration<double>> times;
};

}  // namespace brainfuck
