#pragma once

#include "util/vec2.hpp"
#include "util/vec3.hpp"

struct Vertex
{
  constexpr Vertex() : pos(0.0, 0.0, 0.0), normal(0.0, 0.0, 0.0), tex_coords(0.0, 0.0)
  {}
  constexpr Vertex(const glm::vec3& pos) : pos(pos), normal(0.0, 0.0, 0.0), tex_coords(0.0, 0.0)
  {}
  constexpr Vertex(const glm::vec3& pos, const glm::vec3& normal) : pos(pos), normal(normal), tex_coords(0.0, 0.0)
  {}
  constexpr Vertex(const glm::vec3& pos, const glm::vec3& normal, glm::vec2 tex_coords) : pos(pos), normal(normal), tex_coords(tex_coords)
  {}
  glm::vec3 pos;
  glm::vec3 normal;
  glm::vec2 tex_coords;
};
