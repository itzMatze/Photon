#pragma once

#include <random>
#include "glm/glm.hpp"

class RandomGenerator
{
public:
    RandomGenerator();

    inline float random_num()
    {return distribution(generator);}

    glm::vec3 random_in_unit_sphere();
    glm::vec3 random_cosine_weighted_hemisphere(const glm::vec3 n);

private:
    std::uniform_real_distribution<float> distribution;
    std::mt19937 generator;
};
