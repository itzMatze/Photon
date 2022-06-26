#pragma once

#include <utility>
#include <vector>

#include "Hitable.h"

class HitableList : public Hitable
{
public:
    HitableList() = default;
    explicit HitableList(std::vector<std::shared_ptr<Hitable>>& objects) : objects(std::move(objects))
    {
    }
    bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const override;
    bool bounding_box(Aabb& box) const override;
    std::vector<std::shared_ptr<Hitable>> objects;
};
