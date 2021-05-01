#pragma once
#ifndef RAY_TRACER_CPP_DEFINES_H
#define RAY_TRACER_CPP_DEFINES_H
#include "glm.hpp"

struct Vertex
{
    glm::vec3 position;
    glm::vec2 uv;
    glm::vec4 color;
};

struct Color
{
    unsigned char r, g, b, a;
};

#endif //RAY_TRACER_CPP_DEFINES_H