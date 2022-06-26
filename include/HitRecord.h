#pragma once

#include <memory>

class Material;

struct HitRecord
{
    glm::vec3 p;
    glm::vec3 normal;
    glm::vec2 uv;
    float t;
    Material* mat;
};
