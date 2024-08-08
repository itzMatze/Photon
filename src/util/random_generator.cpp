#include "util/random_generator.hpp"
#include <random>

#define USE_DETERMINISTIC_SEED

namespace rng
{
std::mt19937 get_generator()
{
#if defined(USE_DETERMINISTIC_SEED)
    return std::mt19937(42);
#else
    std::random_device rd;
    std::uniform_int_distribution<uint32_t> dis(0, 20000);
    return std::mt19937(dis(rd));
#endif
}

struct RandomGenerator
{
    std::uniform_real_distribution<float> distribution;
    std::mt19937 generator;
};

RandomGenerator rg{std::uniform_real_distribution<float>(0.0f, 1.0f), get_generator()};

float random_float(float lower_bound, float upper_bound)
{
    return rg.distribution(rg.generator) * (upper_bound - lower_bound) + lower_bound;
}

int32_t random_int32(int32_t lower_bound, int32_t upper_bound)
{
    return random_float(lower_bound, upper_bound);
}
} // namespace rng

