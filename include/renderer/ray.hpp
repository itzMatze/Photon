#pragma once
#include <limits>
#include "util/vec3.hpp"

#define RAY_START_OFFSET 0.0001f

struct RayConfig
{
  float max_t = std::numeric_limits<float>::max();
  bool anyhit = false;
  bool backface_culling = true;
};

class Ray
{
public:
  Ray(const glm::vec3& origin, const glm::vec3& dir, const RayConfig config = RayConfig());
  glm::vec3 at(float t) const;
  const glm::vec3& get_dir() const;
  const glm::vec3& get_inv_dir() const;
  void set_dir(const glm::vec3& dir);

  glm::vec3 origin;
  RayConfig config;

private:
  glm::vec3 dir;
  glm::vec3 inv_dir;
};

