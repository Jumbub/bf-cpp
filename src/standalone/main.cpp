#include <iostream>
#include "../brainfuck/go.h"

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cout << "Usage: brainfuck <filename>\n";
    return 1;
  }

  return (int)brainfuck::go(argv[1]);
}
