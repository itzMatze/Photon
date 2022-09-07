#include "RandomGenerator.h"

RandomGenerator::RandomGenerator()
{
    //std::random_device rd;
    //std::uniform_real_distribution<float> dis(0, 20000);
    distribution = std::uniform_real_distribution<float>(0.0f, 1.0f);
    generator = std::mt19937(0/*dis(rd)*/);
}

glm::vec3 RandomGenerator::random_in_unit_sphere()
{
    glm::vec3 p;
    do
    {
        p = 2.0f * glm::vec3(random_num(), random_num(), random_num()) - glm::vec3(1.0f, 1.0f, 1.0f);
    }
    while (glm::length(p) >= 1.0f);
    return p;
}

glm::vec3 RandomGenerator::random_cosine_weighted_hemisphere(const glm::vec3 n)
{
    float a = 1.0f - 2.0f * random_num();
    float b = std::sqrt(1.0f - a * a);
    float phi = 2.0f * M_PI * random_num();
    return glm::vec3(n.x + b*cos(phi), n.y + b*sin(phi), n.z + a);
    float pdf = a / M_PI;
}