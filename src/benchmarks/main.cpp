#include <iostream>
#include "benchmark/benchmark.h"
#include "file.h"

constexpr auto mandelbrot_filename = "tests/mandelbrot.b";

static void file_read_1(benchmark::State& state) {
  for (auto _ : state) {
    auto output = file::read_1(mandelbrot_filename);
    benchmark::DoNotOptimize(output);
  }
}
BENCHMARK(file_read_1);

static void file_read_2(benchmark::State& state) {
  for (auto _ : state) {
    auto output = file::read_2(mandelbrot_filename);
    benchmark::DoNotOptimize(output);
  }
}
BENCHMARK(file_read_2);

static void file_read_3(benchmark::State& state) {
  for (auto _ : state) {
    auto output = file::read_3(mandelbrot_filename);
    benchmark::DoNotOptimize(output);
  }
}
BENCHMARK(file_read_3);

static void file_read_4(benchmark::State& state) {
  for (auto _ : state) {
    auto output = file::read_4(mandelbrot_filename);
    benchmark::DoNotOptimize(output);
  }
}
BENCHMARK(file_read_4);

static void file_read_5(benchmark::State& state) {
  for (auto _ : state) {
    auto output = file::read_5(mandelbrot_filename);
    benchmark::DoNotOptimize(output);
  }
}
BENCHMARK(file_read_5);

static void file_read_6(benchmark::State& state) {
  for (auto _ : state) {
    auto output = file::read_6(mandelbrot_filename);
    benchmark::DoNotOptimize(output);
  }
}
BENCHMARK(file_read_6);

static void file_read_7(benchmark::State& state) {
  for (auto _ : state) {
    auto output = file::read_7(mandelbrot_filename);
    benchmark::DoNotOptimize(output);
  }
}
BENCHMARK(file_read_7);

static void file_read_8(benchmark::State& state) {
  for (auto _ : state) {
    auto output = file::read_8(mandelbrot_filename);
    benchmark::DoNotOptimize(output);
  }
}
BENCHMARK(file_read_8);

BENCHMARK_MAIN();
