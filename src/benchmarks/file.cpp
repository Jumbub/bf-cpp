#include "file.h"

#include "benchmark/benchmark.h"

#include <string.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace file {

// *modified* https://stackoverflow.com/a/2602060
char* read_1(const char* filename) {
  std::ifstream in(filename);
  std::string contents((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
  return strdup(contents.c_str());
}

// https://stackoverflow.com/a/2602060
std::string read_2(const char* filename) {
  std::ifstream in(filename);
  return {(std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>()};
}

// https://stackoverflow.com/a/18816228
std::vector<char> read_3(const char* filename) {
  std::ifstream file(filename, std::ios::binary | std::ios::ate);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<char> buffer(size);
  if (file.read(buffer.data(), size)) {
  }
  return buffer;
}

// *modified* https://stackoverflow.com/a/18816228
std::vector<char> read_4(const char* filename) {
  std::ifstream file(filename);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<char> buffer(size);
  if (file.read(buffer.data(), size)) {
  }
  return buffer;
}

// https://stackoverflow.com/a/2602258
std::string read_5(const char* filename) {
  std::ifstream t(filename);
  t.seekg(0, std::ios::end);
  size_t size = t.tellg();
  std::string buffer(size, ' ');
  t.seekg(0);
  t.read(&buffer[0], size);
  return buffer;
}

// https://stackoverflow.com/a/2602060
std::string read_6(const char* filename) {
  std::ifstream t(filename);
  std::string str;

  t.seekg(0, std::ios::end);
  str.reserve(t.tellg());
  t.seekg(0, std::ios::beg);

  str.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

  return str;
}

// https://stackoverflow.com/a/2602258
std::string read_7(const char* filename) {
  std::ifstream t(filename);
  std::stringstream buffer;
  buffer << t.rdbuf();
  return buffer.str();
}

// https://stackoverflow.com/a/22059317
char* read_8(const char* filename) {
  FILE* fileptr;
  char* buffer;
  long filelen;

  fileptr = fopen(filename, "rb");  // Open the file in binary mode
  fseek(fileptr, 0, SEEK_END);      // Jump to the end of the file
  filelen = ftell(fileptr);         // Get the current byte offset in the file
  rewind(fileptr);                  // Jump back to the beginning of the file

  buffer = (char*)malloc(filelen * sizeof(char));  // Enough memory for the file
  fread(buffer, filelen, 1, fileptr);              // Read in the entire file
  fclose(fileptr);                                 // Close the file
  return buffer;
}

}  // namespace file
