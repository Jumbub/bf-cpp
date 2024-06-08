include(FetchContent)
FetchContent_Declare(
  doctest
  GIT_REPOSITORY "https://github.com/doctest/doctest.git"
  GIT_TAG "v2.4.11"
)
FetchContent_MakeAvailable(doctest)

add_executable(tests main.cpp utils.cpp file.cpp go.cpp)
target_link_libraries(tests brainfuck doctest::doctest)
