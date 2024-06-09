#include "../brainfuck/file.h"
#include "../brainfuck/parse.h"
#include "benchmark/benchmark.h"

template <class... Args>
void parse(benchmark::State& state, Args&&... args) {
  auto args_tuple = std::make_tuple(std::move(args)...);
  const std::string filename = std::get<0>(args_tuple);

  const auto code = brainfuck::read(filename).value();
  for (auto _ : state) {
    auto bytecode = brainfuck::parse(code);
    benchmark::DoNotOptimize(bytecode);
  }
}

BENCHMARK_CAPTURE(parse, mandelbrot, "samples/mandelbrot.b");
BENCHMARK_CAPTURE(parse, cgbfi, "samples/cgbfi.b");
BENCHMARK_CAPTURE(parse, dbfi, "samples/dbfi.b");
BENCHMARK_CAPTURE(parse, hello_world, "samples/hello_world.b");
BENCHMARK_CAPTURE(parse, rot13, "samples/rot13.b");
