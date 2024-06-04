#pragma once

#include <optional>
#include <string>
#include <vector>

namespace brainfuck {

std::optional<std::vector<char>> read(const std::string filename);

}  // namespace brainfuck
