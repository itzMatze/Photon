#pragma once

#include "renderer/color.hpp"
#include "util/vec2.hpp"
#include "util/vec3.hpp"

struct Vertex
{
  constexpr Vertex() : pos(0.0, 0.0, 0.0), normal(0.0, 0.0, 0.0), color(0), tex_coords(0.0, 0.0)
  {}
  constexpr Vertex(const glm::vec3& pos) : pos(pos), normal(0.0, 0.0, 0.0), color(0), tex_coords(0.0, 0.0)
  {}
  constexpr Vertex(const glm::vec3& pos, const glm::vec3& normal) : pos(pos), normal(normal), color(0), tex_coords(0.0, 0.0)
  {}
  constexpr Vertex(const glm::vec3& pos, const glm::vec3& normal, glm::vec2 tex_coords) : pos(pos), normal(normal), color(0), tex_coords(tex_coords)
  {}
  constexpr Vertex(const glm::vec3& pos, const glm::vec3& normal, const Color& color) : pos(pos), normal(normal), color(color), tex_coords(0.0, 0.0)
  {}
  constexpr Vertex(const glm::vec3& pos, const glm::vec3& normal, const Color& color, glm::vec2 tex_coords) : pos(pos), normal(normal), color(color), tex_coords(tex_coords)
  {}
  glm::vec3 pos;
  glm::vec3 normal;
  Color color;
  glm::vec2 tex_coords;
};
