add_compile_options(-Wall -Wextra -Werror -Wpedantic -Wsign-conversion -Wconversion -march=native)
add_library(brainfuck SHARED file.cpp parse.cpp)
