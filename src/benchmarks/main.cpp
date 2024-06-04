#include <iostream>
#include "../brainfuck/execute.h"
#include "../brainfuck/file.h"
#include "../brainfuck/parse.h"
#include "benchmark/benchmark.h"

constexpr auto mandelbrot_filename = "src/tests/programs/mandelbrot.b";

static void file_read(benchmark::State& state) {
  for (auto _ : state) {
    auto maybeContents = brainfuck::read(mandelbrot_filename);
    if (!maybeContents.has_value()) {
      state.SkipWithError("bad filename");
      return;
    }

    auto contents = maybeContents.value();
    if (contents[11593] != ']') {
      state.SkipWithError("bad output");
    }

    benchmark::DoNotOptimize(contents);
  }
}
BENCHMARK(file_read);

static void brainfuck_parse(benchmark::State& state) {
  const auto code = brainfuck::read(mandelbrot_filename);
  if (!code.has_value()) {
    state.SkipWithError("bad filename");
    return;
  }

  for (auto _ : state) {
    auto instructions = brainfuck::parse(code.value());
    benchmark::DoNotOptimize(instructions);
  }
}
BENCHMARK(brainfuck_parse);

/* static void brainfuck_execute(benchmark::State& state) { */
/*   const auto code = brainfuck::read(mandelbrot_filename); */
/*   if (!code.has_value()) { */
/*     state.SkipWithError("bad filename"); */
/*     return; */
/*   } */
/*   auto instructions = brainfuck::parse(code.value()); */

/*   for (auto _ : state) { */
/*     brainfuck::execute(instructions); */
/*   } */
/* } */
/* BENCHMARK(brainfuck_execute); */

BENCHMARK_MAIN();
