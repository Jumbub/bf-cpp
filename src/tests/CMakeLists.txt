include(FetchContent)
FetchContent_Declare(
  google_test
  GIT_REPOSITORY "https://github.com/google/googletest.git"
  GIT_TAG "v1.14.0"
)
FetchContent_Declare(
  approval_tests
  GIT_REPOSITORY "https://github.com/approvals/ApprovalTests.cpp"
  GIT_TAG "v.10.13.0"
)
FetchContent_MakeAvailable(google_test approval_tests)

add_executable(tests main.cpp file.cpp go.cpp parse.cpp)
# target_precompile_headers(tests PRIVATE pch.h)
target_link_libraries(tests brainfuck gtest ApprovalTests::ApprovalTests)
