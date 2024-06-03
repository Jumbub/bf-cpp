#include <iostream>
#include "benchmark/benchmark.h"
#include "file.h"

constexpr auto mandelbrot_filename = "tests/mandelbrot.b";

static void file_read(benchmark::State& state) {
  for (auto _ : state) {
    auto output = file::read(mandelbrot_filename);
    if (output[11593] != ']') {
      state.SkipWithError("bad output");
    }
    benchmark::DoNotOptimize(output);
  }
}
BENCHMARK(file_read);

BENCHMARK_MAIN();
