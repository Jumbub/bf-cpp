#include "../brainfuck/go.h"
#include "benchmark/benchmark.h"
#include "utils.h"

template <class... Args>
void go(benchmark::State& state, Args&&... args) {
  auto args_tuple = std::make_tuple(std::move(args)...);
  const std::string filename = std::get<0>(args_tuple);
  const std::string input = std::get<1>(args_tuple);

  for (auto _ : state) {
    const auto stop = startCapturingIO(input);

    auto output = brainfuck::go(filename);
    benchmark::DoNotOptimize(output);

    stop();
  }
}

BENCHMARK_CAPTURE(go, mandelbrot, "samples/mandelbrot.b", "")->Unit(benchmark::kMillisecond);
