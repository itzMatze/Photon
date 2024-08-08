#pragma once
#include <limits>
#include "util/vec3.hpp"

#define RAY_START_OFFSET 0.0001

struct RayConfig
{
  float max_t = std::numeric_limits<float>::max();
  bool anyhit = false;
  bool backface_culling = true;
};

class Ray
{
public:
  Ray(const cm::Vec3& origin, const cm::Vec3& dir, const RayConfig config = RayConfig());
  cm::Vec3 at(float t) const;
  const cm::Vec3& get_dir() const;
  const cm::Vec3& get_inv_dir() const;
  void set_dir(const cm::Vec3& dir);

  cm::Vec3 origin;
  RayConfig config;

private:
  cm::Vec3 dir;
  cm::Vec3 inv_dir;
};

