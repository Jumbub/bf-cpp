add_compile_options(-Wall -Wextra -Werror -Wsign-conversion -Wconversion -march=native -Ofast)
add_library(brainfuck-execute SHARED execute.cpp)
add_library(brainfuck SHARED instruction.cpp format.cpp file.cpp parse.cpp go.cpp)
target_link_libraries(brainfuck brainfuck-execute)
