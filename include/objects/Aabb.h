#pragma once

#include "glm/glm.hpp"

class Ray;

inline float ffmin(float a, float b)
{
    return a < b ? a : b;
}

inline float ffmax(float a, float b)
{
    return a > b ? a : b;
}

class Aabb
{
public:
    Aabb() {}
    Aabb(const glm::vec3& a, const glm::vec3& b) : _min(a), _max(b) {}
    Aabb(Aabb box0, Aabb box1)
    {
        glm::vec3 small(glm::min(box0.min(), box1.min()));
        glm::vec3 big(glm::max(box0.max(), box1.max()));
        _min = small;
        _max = big;
    }

    glm::vec3 min() const
    {
        return _min;
    }

    glm::vec3 max() const
    {
        return _max;
    }

    bool hit(const Ray& r, float tmin, float tmax) const;

private:
    glm::vec3 _min;
    glm::vec3 _max;
};
