#pragma once

#include <memory>
#include "glm/glm.hpp"

class Material;

struct RayPayload
{
    RayPayload(uint32_t depth, uint32_t wavelength) : depth(depth), wavelength(wavelength)
    {}
    uint32_t depth;
    uint32_t wavelength;
    glm::vec3 p;
    glm::vec3 normal;
    glm::vec2 uv;
    float t;
    Material* mat;
};
