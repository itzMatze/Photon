#ifndef RAY_TRACING_IOW_SPHERE_H
#define RAY_TRACING_IOW_SPHERE_H

#include <utility>

#include "Hitable.h"

class Sphere : public Hitable
{
public:
    Sphere() = default;
    Sphere(glm::vec3 center, float radius, std::shared_ptr<Material> mat) : center(center), radius(radius), mat(std::move(mat))
    {}
    bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const override;
    glm::vec3 center;
    float radius;
    std::shared_ptr<Material> mat;
};

#endif //RAY_TRACING_IOW_SPHERE_H
