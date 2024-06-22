#include "profile.h"

#include <iostream>
#include <print>
#include "format.h"

using namespace std::chrono;

brainfuck::Profile::Profile(const std::vector<Instruction>& instructions)
    : instructions(instructions),
      executions(instructions.size(), 0),
      time(high_resolution_clock::now()),
      times(instructions.size(), duration_cast<duration<double>>(time - time)) {}

void brainfuck::Profile::instruction(const Instruction* instruction) {
  this->executions[static_cast<size_t>(instruction - &instructions[0])] += 1;
  // this->times[index] += high_resolution_clock::now() - this->time;
  // this->time = high_resolution_clock::now();
};

brainfuck::Profile::~Profile() {
  for (size_t i = 0; i < instructions.size(); i++) {
    const auto instruction = instructions[i];
    std::cout << std::format(
        "{} ! {:010} T {:010}\n", formatInstruction(instruction), this->executions[i],
        duration_cast<nanoseconds>(this->times[i]).count());
  }
};
