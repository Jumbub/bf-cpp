#pragma once

#include <optional>
#include <string>
#include <vector>

namespace brainfuck {

std::optional<std::vector<char>> read(const std::string filename);
void write(const std::string filename, const char* unknown, const std::streamsize size);

}  // namespace brainfuck
