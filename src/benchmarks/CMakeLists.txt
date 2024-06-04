set(BENCHMARK_ENABLE_TESTING NO)

include(FetchContent)
FetchContent_Declare(
  google_benchmark
  GIT_REPOSITORY "https://github.com/google/benchmark.git"
  GIT_TAG "v1.8.4"
)
FetchContent_MakeAvailable(google_benchmark)

add_executable(benchmarks main.cpp file.cpp parse.cpp execute.cpp)
target_link_libraries(benchmarks benchmark::benchmark)
