#pragma once

#include <utility>

#include "Hitable.h"

class Sphere : public Hitable
{
public:
    Sphere() = default;
    Sphere(glm::vec3 center, float radius, std::shared_ptr<Material> mat) : center(center), radius(radius), mat(mat)
    {}
    bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const override;
    bool bounding_box(Aabb& box) const override;
    glm::vec2 get_uv(const glm::vec3& p) const;
    glm::vec3 center;
    float radius;
    std::shared_ptr<Material> mat;
};
