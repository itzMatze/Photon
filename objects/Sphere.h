#ifndef RAY_TRACING_IOW_SPHERE_H
#define RAY_TRACING_IOW_SPHERE_H

#include "Hitable.h"

class Sphere : public Hitable
{
public:
    Sphere() = default;
    Sphere(glm::vec3 center, float radius, Material& mat) : center(center), radius(radius), mat(mat)
    {}
    bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const override;
    glm::vec3 center;
    float radius;
    Material& mat;
};

#endif //RAY_TRACING_IOW_SPHERE_H
