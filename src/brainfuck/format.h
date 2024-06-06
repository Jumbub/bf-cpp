#pragma once

#include <format>
#include "parse.h"

/* template <> */
/* struct std::formatter<brainfuck::Instruction> : std::formatter<string_view> { */
/*   auto format(const brainfuck::Instruction& col, std::format_context& ctx) const { */
/*     return std::formatter<string_view>::format(std::format("hi"), ctx); */
/*   } */
/* }; */

/* template <> */
/* struct std::formatter<brainfuck::Type> : std::formatter<std::string_view> { */
/*   auto format(const brainfuck::Type& value, std::format_context& ctx) const; */
/* }; */

template <>
struct std::formatter<brainfuck::Instruction> : std::formatter<std::string_view> {
  auto format(const brainfuck::Instruction& value, std::format_context& ctx) const;
};

/* template <> */
/* struct std::formatter<std::vector<brainfuck::Instruction>> : std::formatter<std::string_view> { */
/*   auto format(const std::vector<brainfuck::Instruction>& value, std::format_context& ctx) const; */
/* }; */
