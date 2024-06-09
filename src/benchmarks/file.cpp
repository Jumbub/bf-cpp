#include "../brainfuck/file.h"
#include "benchmark/benchmark.h"

template <class... Args>
void file(benchmark::State& state, Args&&... args) {
  auto args_tuple = std::make_tuple(std::move(args)...);
  const std::string filename = std::get<0>(args_tuple);

  for (auto _ : state) {
    auto result = brainfuck::read(filename);
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK_CAPTURE(file, mandelbrot, "samples/mandelbrot.b");
BENCHMARK_CAPTURE(file, hello_world, "samples/hello_world.b");
BENCHMARK_CAPTURE(file, rot13, "samples/rot13.b");
BENCHMARK_CAPTURE(file, empty_file, "samples/empty_file.b");
BENCHMARK_CAPTURE(file, non_existent_file, "non_existent_file");
