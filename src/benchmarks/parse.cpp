#include "../brainfuck/file.h"
#include "../brainfuck/parse.h"
#include "benchmark/benchmark.h"

static void parse_read_empty(benchmark::State& state) {
  const auto code = brainfuck::read("samples/tests/empty.b").value();
  for (auto _ : state) {
    auto bytecode = brainfuck::parse({});
    benchmark::DoNotOptimize(bytecode);
  }
}
BENCHMARK(parse_read_empty);

static void parse_read_hello_world(benchmark::State& state) {
  const auto code = brainfuck::read("samples/hello_world.b").value();
  for (auto _ : state) {
    auto bytecode = brainfuck::parse(code);
    benchmark::DoNotOptimize(bytecode);
  }
}
BENCHMARK(parse_read_hello_world);

static void parse_read_mandelbrot(benchmark::State& state) {
  const auto code = brainfuck::read("samples/mandelbrot.b").value();
  for (auto _ : state) {
    auto bytecode = brainfuck::parse(code);
    benchmark::DoNotOptimize(bytecode);
  }
}
BENCHMARK(parse_read_mandelbrot);
