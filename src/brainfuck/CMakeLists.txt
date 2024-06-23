add_compile_options(-Wall -Wextra -Werror -Wsign-conversion -Wconversion -march=native -Ofast)
add_library(brainfuck SHARED types.cpp format.cpp file.cpp execute.cpp parse.cpp)
