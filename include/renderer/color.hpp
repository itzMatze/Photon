#pragma once
#include <cstdint>
#include "util/vec3.hpp"
#include "util/vec4.hpp"

class Color
{
public:
  constexpr Color() : value(0.0), alpha(0.0)
  {}
  explicit constexpr Color(const glm::vec3& rgb) : value(rgb), alpha(1.0)
  {}
  explicit constexpr Color(const glm::vec4& rgb) : value(rgb), alpha(rgb.a)
  {}
  constexpr Color(float r, float g, float b) : value(r, g, b), alpha(1.0)
  {}
  constexpr Color(float r, float g, float b, float a) : value(r, g, b), alpha(a)
  {}
  constexpr Color(uint32_t hex_color)
  {
    value.x = float(hex_color & 0x000000ff) / 255.0f;
    hex_color >>= 8;
    value.y = float(hex_color & 0x000000ff) / 255.0f;
    hex_color >>= 8;
    value.z = float(hex_color & 0x000000ff) / 255.0f;
    hex_color >>= 8;
    alpha = float(hex_color) / 255.0f;
  }

  uint32_t get_hex_color() const;
  static Color get_random_color();

  glm::vec3 value;
  float alpha;
};

