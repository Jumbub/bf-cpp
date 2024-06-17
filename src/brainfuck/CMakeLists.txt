add_compile_options(-Wall -Wextra -Werror -Wpedantic -Wsign-conversion -Wconversion -march=native)
add_library(brainfuck SHARED types.cpp format.cpp file.cpp parse.cpp execute.cpp debug.cpp)
