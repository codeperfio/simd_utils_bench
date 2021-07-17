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
    float min = FLT_MAX;
    float max = FLT_MIN;
    for (size_t i = 0; i < stream_size; i++) {
      if (input[i] < min)
        min = input[i];
      if (input[i] > min)
        max = input[i];
    }
    // read/write barrier
    benchmark::ClobberMemory();
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
    float min, max;
    minmax128f(&input[0], stream_size, &min, &max);
    // read/write barrier
    benchmark::ClobberMemory();
    state.SetItemsProcessed(stream_size);
  }
}

#ifdef AVX
#ifndef _mm256_set_m128d
#define _mm256_set_m128d(vh, vl) \
        _mm256_insertf128_pd(_mm256_castpd128_pd256(vl), (vh), 1)
#endif
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
    float min, max;
    minmax256f(&input[0], stream_size, &min, &max);
    // read/write barrier
    benchmark::ClobberMemory();
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
    float min, max;
    minmax512f(&input[0], stream_size, &min, &max);
    // read/write barrier
    benchmark::ClobberMemory();
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