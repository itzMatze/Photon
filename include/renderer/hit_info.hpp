#pragma once

#include <limits>
#include "util/vec2.hpp"
#include "util/vec3.hpp"

struct HitInfo
{
  cm::Vec3 pos = cm::Vec3(0.0);
  cm::Vec3 geometric_normal = cm::Vec3(0.0);
  cm::Vec3 normal = cm::Vec3(0.0);
  cm::Vec2 bary = cm::Vec2(0.0);
  cm::Vec2 tex_coords = cm::Vec2(0.0);
  float t = std::numeric_limits<float>::max();
  int32_t material_idx = -1;
};
