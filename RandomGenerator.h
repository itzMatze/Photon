#ifndef RAY_TRACER_CPP_RANDOMGENERATOR_H
#define RAY_TRACER_CPP_RANDOMGENERATOR_H

#include <random>
#include <glm.hpp>

class RandomGenerator
{
public:
    RandomGenerator();

    inline float random_num()
    {return distribution(generator);}

    glm::vec3 random_in_unit_sphere();

private:
    std::uniform_real_distribution<float> distribution;
    std::default_random_engine generator;
};

#endif //RAY_TRACER_CPP_RANDOMGENERATOR_H
