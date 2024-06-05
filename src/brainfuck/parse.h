#pragma once

#include <format>
#include <iostream>
#include <string>
#include <vector>

namespace brainfuck {

enum Type {
  NOOP = 'x',
  READ = ',',
  WRITE = '.',
  MUTATE_DATA = '+',
  MUTATE_DATA_POINTER = '>',
  MUTATE_INSTRUCTION_POINTER_IF_ZERO = '[',
  MUTATE_INSTRUCTION_POINTER_IF_NOT_ZERO = ']'
};

struct Instruction {
  Type type = NOOP;
  int value = 0;
};

std::vector<Instruction> parse(std::vector<char> full);

}  // namespace brainfuck

template <>
struct std::formatter<brainfuck::Type> : std::formatter<std::string_view> {
  auto format(const brainfuck::Type& value, std::format_context& ctx) const;
};

template <>
struct std::formatter<brainfuck::Instruction> : std::formatter<std::string_view> {
  auto format(const brainfuck::Instruction& value, std::format_context& ctx) const;
};
