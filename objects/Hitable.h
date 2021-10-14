#ifndef RAY_TRACING_IOW_HITABLE_H
#define RAY_TRACING_IOW_HITABLE_H

#include <memory>

#include "Ray.h"
#include "HitRecord.h"

class Material;

class Hitable
{
public:
    virtual bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const = 0;
};

#endif //RAY_TRACING_IOW_HITABLE_H
