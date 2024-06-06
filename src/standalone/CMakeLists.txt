add_executable(standalone main.cpp)
target_link_libraries(standalone brainfuck)
set_target_properties(standalone PROPERTIES OUTPUT_NAME "brainfuck")
