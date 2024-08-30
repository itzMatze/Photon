#pragma once
#include <cstdint>
#include <limits>
#include <random>

class RandomGenerator
{
public:
  RandomGenerator(uint32_t seed = 42);
  float random_float(float lower_bound = 0.0f, float upper_bound = 1.0f);
  int32_t random_int32(int32_t lower_bound = std::numeric_limits<int32_t>::min(), int32_t upper_bound = std::numeric_limits<int32_t>::max());

private:
  std::uniform_real_distribution<float> distribution;
  std::mt19937 generator;
};

namespace rng
{
float random_float(float lower_bound = 0.0f, float upper_bound = 1.0f);

// generate random int in [lower_bound, upper_bound - 1]
int32_t random_int32(int32_t lower_bound = std::numeric_limits<int32_t>::min(), int32_t upper_bound = std::numeric_limits<int32_t>::max());
} // namespace rng

