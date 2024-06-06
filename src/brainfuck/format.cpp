#include "format.h"

/* auto std::formatter<brainfuck::Type>::format(const brainfuck::Type& value, std::format_context& ctx) const { */
/*   return std::formatter<string_view>::format(std::format("{}", value), ctx); */
/* }; */

auto std::formatter<brainfuck::Instruction>::format(const brainfuck::Instruction& value, std::format_context& ctx)
    const {
  return std::formatter<string_view>::format(std::format("hi"), ctx);
};

/* auto std::formatter<std::vector<brainfuck::Instruction>>::format( */
/*     const std::vector<brainfuck::Instruction>& value, */
/*     std::format_context& ctx) const { */
/*   std::string acc; */
/*   for (const auto instruction : value) { */
/*     acc += std::format("{}", value); */
/*   } */
/*   return std::formatter<string_view>::format(acc, ctx); */
/* }; */
