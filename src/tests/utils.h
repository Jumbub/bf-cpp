#pragma once

#include <functional>
#include <optional>
#include <string>

void REQUIRE_SNAPSHOT(std::string path, std::string received);

using StopCaptureIO = std::function<std::string(void)>;
StopCaptureIO startCapturingIO(std::optional<std::string> input);
