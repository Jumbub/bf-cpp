#pragma once

#include <functional>
#include <optional>
#include <string>

using StopCaptureIO = std::function<std::string(void)>;
StopCaptureIO startCapturingIO(std::optional<std::string> input);
