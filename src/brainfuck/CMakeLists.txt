# add_compile_options(-Wall -Wextra -Werror -Wsign-conversion -Wconversion -march=native -Ofast -fprofile-generate)
add_compile_options(-Wall -Wextra -Werror -Wsign-conversion -Wconversion -march=native -Ofast -fprofile-use)
add_library(brainfuck SHARED types.cpp format.cpp file.cpp execute.cpp parse.cpp)
target_link_libraries(brainfuck gcov)
