#pragma once

#include "renderer/ray.hpp"
#include "util/vec3.hpp"

class AABB
{
public:
  AABB();
  AABB(const cm::Vec3& min, const cm::Vec3& max);
  bool intersect(const Ray& ray) const;
  bool intersect(const AABB& other) const;

  cm::Vec3 min;
  cm::Vec3 max;
};
