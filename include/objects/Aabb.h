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
        glm::vec3 small(ffmin(box0.min().x, box1.min().x), ffmin(box0.min().y, box1.min().y), ffmin(box0.min().z, box1.min().z));
        glm::vec3 big(ffmax(box0.max().x, box1.max().x), ffmax(box0.max().y, box1.max().y), ffmax(box0.max().z, box1.max().z));
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
