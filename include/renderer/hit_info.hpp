#pragma once

#include <limits>
#include "util/vec2.hpp"
#include "util/vec3.hpp"

struct HitInfo
{
  glm::vec3 pos = glm::vec3(0.0);
  glm::vec3 geometric_normal = glm::vec3(0.0);
  glm::vec3 normal = glm::vec3(0.0);
  glm::vec2 bary = glm::vec2(0.0);
  glm::vec2 tex_coords = glm::vec2(0.0);
  float t = std::numeric_limits<float>::max();
  int32_t material_idx = -1;
};
