#pragma once

#include <vector>

#include "Hitable.h"

class BvhNode : public Hitable
{
public:
    BvhNode() {}
    BvhNode(std::vector<std::shared_ptr<Hitable>>::iterator begin, std::vector<std::shared_ptr<Hitable>>::iterator end);

    virtual bool hit(const Ray& r, float tmin, float tmax, RayPayload& rp) const override;
    virtual bool bounding_box(Aabb& box) const override;
    std::shared_ptr<Hitable> left;
    std::shared_ptr<Hitable> right;
    Aabb box;
};
