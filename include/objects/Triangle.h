#pragma once

#include "Hitable.h"

class Triangle : public Hitable
{
public:
    Triangle() = default;
    Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, std::shared_ptr<Material> mat) : p0(v0), p1(v1), p2(v2), n(glm::normalize(glm::cross(v1 - v0, v2 - v0))), mat(mat)
    {}
    bool hit(const Ray& r, float t_min, float t_max, RayPayload& rp) const override;
    bool bounding_box(Aabb& box) const override;
    
    glm::vec3 p0, p1, p2, n;
    std::shared_ptr<Material> mat;
};
