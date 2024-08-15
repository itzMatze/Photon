#pragma once

#include <limits>
#include "util/vec2.hpp"
#include "util/vec3.hpp"

struct HitInfo
{
  glm::vec3 pos = glm::vec3(0.0);
  glm::vec3 geometric_normal = glm::vec3(0.0);
  glm::vec3 normal = glm::vec3(0.0);
  glm::vec3 albedo = glm::vec3(0.0);
  glm::vec2 bary = glm::vec2(0.0);
  glm::vec2 tex_coords = glm::vec2(0.0);
  float t = std::numeric_limits<float>::max();
  int32_t object_id = -1;
  int32_t material_id = -1;
  bool is_backside = false;

  glm::vec3 get_oriented_face_normal() const
  {
    return (is_backside ? -normal : normal);
  }

  glm::vec3 get_oriented_face_geometric_normal() const
  {
    return (is_backside ? -geometric_normal : geometric_normal);
  }
};
