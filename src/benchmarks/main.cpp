#include <iostream>
#include "../brainfuck/execute.h"
#include "../brainfuck/file.h"
#include "../brainfuck/parse.h"
#include "benchmark/benchmark.h"

constexpr auto mandelbrot_filename = "tests/mandelbrot.b";
constexpr auto bitwidth_filename = "tests/bitwidth.b";
constexpr auto helloworld_filename = "tests/hello-world.b";
constexpr auto simplesthelloworld_filename = "tests/simplest-hello-world.b";

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

static void code_parse(benchmark::State& state) {
  const auto code = file::read(simplesthelloworld_filename);
  for (auto _ : state) {
    const auto instructions = brainfuck::parse(code);
    brainfuck::execute(instructions);
  }
}
BENCHMARK(code_parse);

BENCHMARK_MAIN();
