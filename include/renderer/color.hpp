#pragma once
#include <cstdint>
#include "util/vec3.hpp"
#include "util/vec4.hpp"

class Color
{
public:
  constexpr Color() : value(0.0f)
  {}
  constexpr Color(const glm::vec3& rgb) : value(rgb, 1.0f)
  {}
  constexpr Color(const glm::vec4& rgba) : value(rgba)
  {}
  constexpr Color(float r, float g, float b) : value(r, g, b, 1.0f)
  {}
  constexpr Color(float r, float g, float b, float a) : value(r, g, b, a)
  {}
  constexpr Color(uint32_t hex_color)
  {
    value.r = float(hex_color & 0x000000ff) / 255.0f;
    hex_color >>= 8;
    value.g = float(hex_color & 0x000000ff) / 255.0f;
    hex_color >>= 8;
    value.b = float(hex_color & 0x000000ff) / 255.0f;
    hex_color >>= 8;
    value.a = float(hex_color) / 255.0f;
  }

  uint32_t get_hex_color() const;
  static Color get_random_color();
  template<typename T>
  void operator+=(const T& other)
  { (*this) = (*this) + other; }

  template<typename T>
  void operator-=(const T& other)
  { (*this) = (*this) - other; }

  template<typename T>
  void operator*=(const T& other)
  { (*this) = (*this) * other; }

  template<typename T>
  void operator/=(const T& other)
  { (*this) = (*this) / other; }

  glm::vec4 value;
};

Color operator+(const Color& color, const glm::vec3& other);
Color operator-(const Color& color, const glm::vec3& other);
Color operator*(const Color& color, const glm::vec3& other);
Color operator/(const Color& color, const glm::vec3& other);

Color operator+(const Color& a, const Color& b);
Color operator-(const Color& a, const Color& b);
Color operator*(const Color& a, const Color& b);
Color operator/(const Color& a, const Color& b);
