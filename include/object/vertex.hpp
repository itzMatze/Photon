#pragma once

#include "util/vec2.hpp"
#include "util/vec3.hpp"

struct Vertex
{
  constexpr Vertex() : pos(0.0, 0.0, 0.0), normal(0.0, 0.0, 0.0), tex_coords(0.0, 0.0)
  {}
  constexpr Vertex(const cm::Vec3& pos) : pos(pos), normal(0.0, 0.0, 0.0), tex_coords(0.0, 0.0)
  {}
  constexpr Vertex(const cm::Vec3& pos, const cm::Vec3& normal) : pos(pos), normal(normal), tex_coords(0.0, 0.0)
  {}
  constexpr Vertex(const cm::Vec3& pos, const cm::Vec3& normal, cm::Vec2 tex_coords) : pos(pos), normal(normal), tex_coords(tex_coords)
  {}
  cm::Vec3 pos;
  cm::Vec3 normal;
  cm::Vec2 tex_coords;
};
