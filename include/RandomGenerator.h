#pragma once

#include <random>
#include "glm/glm.hpp"

class RandomGenerator
{
public:
    RandomGenerator(bool seed = false)
    {
        if (seed)
        {
            std::random_device rd;
            std::uniform_int_distribution<uint32_t> dis(0, 20000);
            distribution = std::uniform_real_distribution<float>(0.0f, 1.0f);
            generator = std::mt19937(dis(rd));
        }
        else
        {
            distribution = std::uniform_real_distribution<float>(0.0f, 1.0f);
            generator = std::mt19937(0);
        }
    }

    inline float random_num()
    {return distribution(generator);}

    glm::vec3 random_in_unit_sphere();
    glm::vec3 random_cosine_weighted_hemisphere(const glm::vec3 n);

private:
    std::uniform_real_distribution<float> distribution;
    std::mt19937 generator;
};
