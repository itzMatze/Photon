#pragma once

#include "renderer/ray.hpp"
#include "util/vec3.hpp"

class AABB
{
public:
  AABB();
  AABB(const glm::vec3& min, const glm::vec3& max);
  bool intersect(const Ray& ray) const;
  bool intersect(const AABB& other) const;

  glm::vec3 min;
  glm::vec3 max;
};
