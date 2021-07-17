#include <benchmark/benchmark.h>
#include <float.h>
#include <math.h>
#include <random>
#ifndef _MM_FROUND_RAISE_EXC
#define _MM_FROUND_RAISE_EXC 0
#endif
#include "deps/simd_utils/simd_utils.h"

int64_t min_value = 1;
int64_t min_vec_size = 128;
int64_t max_vec_size = (128 * 1024) / 4;
int64_t step_compression_unit = 2;

// Clang doesn't like the 'X' constraint on `value` and certain GCC versions
// don't like the 'g' constraint. Attempt to placate them both.
#if defined(__clang__)
#define DO_NOT_OPTIMIZE(x) asm volatile("" : : "g"(value) : "memory")
#else
#define DO_NOT_OPTIMIZE(x) asm volatile("" : : "X"(value) : "memory")
#endif

static inline void minmax32f(float *src, int len, float *min_value,
                             float *max_value) {
  float min = FLT_MAX;
  float max = FLT_MIN;
  for (size_t i = 0; i < len; i++) {
    if (src[i] < min)
      min = src[i];
    if (src[i] > min)
      max = src[i];
  }
  *max_value = max;
  *min_value = min;
}

static void generate_arguments_pairs(benchmark::internal::Benchmark *b) {
  for (int64_t vecsize = min_vec_size; vecsize <= max_vec_size;
       vecsize *= step_compression_unit) {
    b = b->ArgPair((double)0, vecsize);
  }
}

static void BM_minmax32f(benchmark::State &state) {
  const int64_t stream_size = state.range(1);
  std::vector<float> input;
  input.resize(stream_size, 0);
  std::mt19937_64 rng;
  rng.seed(std::random_device()());
  std::uniform_real_distribution<float> dist(0, 1);

  for (float &i : input) {
    i = dist(rng);
  }

  while (state.KeepRunning()) {
    std::vector<int> v;
    v.reserve(1);
    float min, max;
    minmax32f(&input[0], stream_size, &min, &max);
    v.push_back(min);
    benchmark::ClobberMemory(); // Force min to be written to memory.

    state.SetItemsProcessed(stream_size);
  }
}

static void BM_minmax128f(benchmark::State &state) {
  const int64_t stream_size = state.range(1);
  std::vector<float> input;
  input.resize(stream_size, 0);
  std::mt19937_64 rng;
  rng.seed(std::random_device()());
  std::uniform_real_distribution<float> dist(0, 1);

  for (float &i : input) {
    i = dist(rng);
  }

  while (state.KeepRunning()) {
    std::vector<int> v;
    float min, max;
    v.reserve(1);
    minmax128f(&input[0], stream_size, &min, &max);
    v.push_back(min);
    benchmark::ClobberMemory(); // Force min to be written to memory.
    state.SetItemsProcessed(stream_size);
  }
}

#ifdef AVX
static void BM_minmax256f(benchmark::State &state) {
  const int64_t stream_size = state.range(1);
  std::vector<float> input;
  input.resize(stream_size, 0);
  std::mt19937_64 rng;
  rng.seed(std::random_device()());
  std::uniform_real_distribution<float> dist(0, 1);

  for (float &i : input) {
    i = dist(rng);
  }

  while (state.KeepRunning()) {
    std::vector<int> v;
    v.reserve(1);
    float min, max;
    minmax256f(&input[0], stream_size, &min, &max);
    v.push_back(min);
    benchmark::ClobberMemory(); // Force min to be written to memory.
    state.SetItemsProcessed(stream_size);
  }
}
#endif

#ifdef AVX512

static void BM_minmax512f(benchmark::State &state) {
  const int64_t stream_size = state.range(1);
  std::vector<float> input;
  input.resize(stream_size, 0);
  std::mt19937_64 rng;
  rng.seed(std::random_device()());
  std::uniform_real_distribution<float> dist(0, 1);

  for (float &i : input) {
    i = dist(rng);
  }

  while (state.KeepRunning()) {
    std::vector<int> v;
    v.reserve(1);
    float min, max;
    minmax512f(&input[0], stream_size, &min, &max);
    v.push_back(min);
    benchmark::ClobberMemory(); // Force min to be written to memory.
    state.SetItemsProcessed(stream_size);
  }
}
#endif

// Register the SSE functions as a benchmark
BENCHMARK(BM_minmax32f)->Apply(generate_arguments_pairs);
BENCHMARK(BM_minmax128f)->Apply(generate_arguments_pairs);

// Register the AVX functions as a benchmark
#ifdef AVX
BENCHMARK(BM_minmax256f)->Apply(generate_arguments_pairs);
#endif

// Register the AVX512 functions as a benchmark
#ifdef AVX512
BENCHMARK(BM_minmax512f)->Apply(generate_arguments_pairs);
#endif

BENCHMARK_MAIN();