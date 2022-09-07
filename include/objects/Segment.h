#pragma once

#include <utility>

#include "Hitable.h"

class Segment : public Hitable
{
public:
    Segment(glm::vec3 p0, glm::vec3 p1, float radius, std::shared_ptr<Material> mat) : p0(p0), p1(p1), radius(radius), mat(mat)
    {}
    bool hit(const Ray& r, float t_min, float t_max, RayPayload& rp) const override;
    bool bounding_box(Aabb& box) const override;
    glm::vec3 p0, p1;
    float radius;
    std::shared_ptr<Material> mat;
};
