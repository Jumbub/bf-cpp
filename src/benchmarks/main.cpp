#include "benchmark/benchmark.h"
#include "file.h"

constexpr auto mandelbrot_filename = "tests/mandelbrot.b";

static void BM_StringCreation(benchmark::State& state) {
  for (auto _ : state)
    std::string fileContents = file::read_1(mandelbrot_filename);
}
// Register the function as a benchmark
BENCHMARK(BM_StringCreation);

// Define another benchmark
static void BM_StringCopy(benchmark::State& state) {
  std::string x = "hello";
  for (auto _ : state)
    std::string copy(x);
}
BENCHMARK(BM_StringCopy);

BENCHMARK_MAIN();
