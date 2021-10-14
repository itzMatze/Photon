#ifndef RAY_TRACING_IOW_HITABLELIST_H
#define RAY_TRACING_IOW_HITABLELIST_H

#include <utility>
#include <vector>

#include "Hitable.h"

class HitableList : public Hitable
{
public:
    HitableList() = default;
    explicit HitableList(std::shared_ptr<std::vector<std::shared_ptr<Hitable>>> objects) : objects(std::move(objects))
    {
    }
    bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const override;
    std::shared_ptr<std::vector<std::shared_ptr<Hitable>>> objects;
};

#endif //RAY_TRACING_IOW_HITABLELIST_H
