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

private:
    std::uniform_real_distribution<float> distribution;
    std::mt19937 generator;
};
