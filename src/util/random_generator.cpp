#include "util/random_generator.hpp"

#define USE_DETERMINISTIC_SEED

uint32_t get_seed()
{
#if defined(USE_DETERMINISTIC_SEED)
  return 42;
#else
  std::random_device rd;
  std::uniform_int_distribution<uint32_t> dis(0, 20000);
  return dis(rd);
#endif
}

RandomGenerator::RandomGenerator(uint32_t seed) : distribution(std::uniform_real_distribution<float>(0.0f, 1.0f)), generator(std::mt19937(seed))
{}

std::mt19937& RandomGenerator::get_generator()
{
  return generator;
}

float RandomGenerator::random_float(float lower_bound, float upper_bound)
{
  return distribution(generator) * (upper_bound - lower_bound) + lower_bound;
}

int32_t RandomGenerator::random_int32(int32_t lower_bound, int32_t upper_bound)
{
  return random_float(lower_bound, upper_bound);
}

glm::vec2 RandomGenerator::random_barycentrics()
{
  glm::vec2 bary(random_float(), random_float());
  if (bary.s + bary.t > 1.0f)
  {
    bary.s = 1.0f - bary.s;
    bary.t = 1.0f - bary.t;
  }
  return bary;
}

namespace rng
{
RandomGenerator rg(get_seed());

float random_float(float lower_bound, float upper_bound)
{
  return random_float(lower_bound, upper_bound);
}

// generate random int in [lower_bound, upper_bound - 1]
int32_t random_int32(int32_t lower_bound, int32_t upper_bound)
{
  return rg.random_int32(lower_bound, upper_bound);
}

RandomGenerator& get_instance()
{
  return rg;
}
} // namespace rng
