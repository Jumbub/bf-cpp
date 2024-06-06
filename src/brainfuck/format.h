#pragma once

#include <format>
#include "parse.h"

template <>
struct std::formatter<brainfuck::Instruction> : std::formatter<std::string_view> {
  auto format(const brainfuck::Instruction& value, std::format_context& ctx) const {
    return std::formatter<string_view>::format(std::format("{}{}", (char)value.type, (int)value.value), ctx);
  };
};

template <>
struct std::formatter<std::vector<brainfuck::Instruction>> : std::formatter<std::string_view> {
  auto format(const std::vector<brainfuck::Instruction>& value, std::format_context& ctx) const {
    std::string acc;
    for (const auto instruction : value) {
      acc += std::format("{}\n", instruction);
    }
    return std::formatter<string_view>::format(acc, ctx);
  };
};
