#pragma once
#include <cstdint>
#include "util/vec3.hpp"

class Color
{
public:
  Color();
  explicit Color(uint32_t hex_color);
  explicit Color(const glm::vec3& rgb);
  Color(float r, float g, float b);
  Color(float r, float g, float b, float a);
  uint32_t get_hex_color() const;
  static Color get_random_color();

  glm::vec3 value;
  float alpha;
};

