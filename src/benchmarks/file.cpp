#include "file.h"

#include <string.h>
#include <fstream>
#include <sstream>
#include <string>

namespace file {

char* read_1(const char* filename) {
  std::ifstream in("FileReadExample.cpp");
  std::string contents((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
  return strdup(contents.c_str());
}

}  // namespace file
