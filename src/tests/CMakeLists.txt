include(FetchContent)
FetchContent_Declare(
  google_test
  GIT_REPOSITORY "https://github.com/google/googletest.git"
  GIT_TAG "v1.14.0"
)
FetchContent_MakeAvailable(google_test)

add_executable(tests main.cpp)
target_link_libraries(tests brainfuck gtest gtest_main)
