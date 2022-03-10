#pragma once

#include <memory>

#include "Ray.h"
#include "HitRecord.h"

class Material;

class Hitable
{
public:
    virtual bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const = 0;
};

// TODO add triangle hitable and mesh of triangles
