#include "../brainfuck/file.h"
#include "benchmark/benchmark.h"

static void file_read_non_existent(benchmark::State& state) {
  for (auto _ : state) {
    auto result = brainfuck::read("this-file-does-not-exist");
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(file_read_non_existent);

static void file_read_empty(benchmark::State& state) {
  for (auto _ : state) {
    auto result = brainfuck::read("programs/empty.b");
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(file_read_empty);

static void file_read_hello_world(benchmark::State& state) {
  for (auto _ : state) {
    auto result = brainfuck::read("programs/hello_world.b");
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(file_read_hello_world);

static void file_read_mandelbrot(benchmark::State& state) {
  for (auto _ : state) {
    auto result = brainfuck::read("programs/mandelbrot.b");
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(file_read_mandelbrot);
