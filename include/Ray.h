#pragma once

#include "glm/glm.hpp"

class Ray
{
public:
    Ray() = default;
    Ray(const glm::vec3& origin, const glm::vec3& dir) : _direction(glm::normalize(dir)), _origin(origin + 0.001f * glm::normalize(dir))
    {    }

    glm::vec3 origin() const
    {
        return _origin;
    }

    glm::vec3 direction() const
    {
        return _direction;
    }

    glm::vec3 point_at_parameter(float t) const
    {
        return _origin + t * _direction;
    }

private:
    glm::vec3 _origin;
    glm::vec3 _direction;
};
