add_compile_options(-Wall -Wextra -Werror -Wsign-conversion -Wconversion -march=native -Ofast)
add_executable(standalone main.cpp)
target_link_libraries(standalone brainfuck)
set_target_properties(standalone PROPERTIES OUTPUT_NAME "brainfuck")
