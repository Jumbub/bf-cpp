#include "utils.h"

#include <stdio.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>

StopCaptureIO startCapturingIO(std::optional<std::string> input) {
  std::shared_ptr<std::stringstream> newStdout = std::make_shared<std::stringstream>();
  std::shared_ptr<std::stringstream> newStdin = std::make_shared<std::stringstream>();
  std::streambuf* oldStdout = std::cout.rdbuf(newStdout->rdbuf());
  std::streambuf* oldStdin = std::cin.rdbuf(newStdin->rdbuf());

  if (input.has_value()) {
    (*newStdin) << input.value();
  }

  return [oldStdout, oldStdin, newStdout, newStdin]() {
    std::cout.rdbuf(oldStdout);
    std::cin.rdbuf(oldStdin);
    return newStdout->str();
  };
}
