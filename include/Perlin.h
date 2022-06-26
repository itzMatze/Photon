#pragma once

#include <vector>

#include "glm/glm.hpp"

class Perlin
{
public:
    Perlin();

    float noise(const glm::vec3& p) const;
    float smooth_noise(const glm::vec3& p) const;
    float turb(const glm::vec3& p, int depth = 7) const;
private:
    std::vector<glm::vec3> ranvec;
    std::vector<int> perm_x;
    std::vector<int> perm_y;
    std::vector<int> perm_z;
};
