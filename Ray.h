#ifndef RAY_TRACING_IOW_RAY_H
#define RAY_TRACING_IOW_RAY_H
#include <glm.hpp>

class Ray
{
public:
    Ray() = default;
    Ray(const glm::vec3& origin, const glm::vec3& dir) : direction(glm::normalize(dir))
    {
        this->origin = origin + 0.001f * direction;
    }

    glm::vec3 point_at_parameter(float t) const
    {
        return origin + t * direction;
    }
    glm::vec3 origin;
    glm::vec3 direction;
};

#endif //RAY_TRACING_IOW_RAY_H
