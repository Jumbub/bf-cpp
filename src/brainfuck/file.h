#pragma once

#include <optional>
#include <string>
#include <vector>

namespace file {

std::optional<std::vector<char>> read(const std::string filename);

}  // namespace file
