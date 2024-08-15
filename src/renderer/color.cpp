#include "renderer/color.hpp"
#include <algorithm>
#include "util/random_generator.hpp"

uint32_t Color::get_hex_color() const
{
  // color values > 1.0 are allowed, but they need to be clamped when the color gets converted to hex representation
  uint32_t hex_color = std::clamp(static_cast<uint64_t>(value.a * 255.999f), uint64_t(0), uint64_t(255));
  hex_color <<= 8;
  hex_color += std::clamp(static_cast<uint64_t>(value.b * 255.999f), uint64_t(0), uint64_t(255));
  hex_color <<= 8;
  hex_color += std::clamp(static_cast<uint64_t>(value.g * 255.999f), uint64_t(0), uint64_t(255));
  hex_color <<= 8;
  hex_color += std::clamp(static_cast<uint64_t>(value.r * 255.999f), uint64_t(0), uint64_t(255));
  return hex_color;
}

Color Color::get_random_color()
{
  uint32_t rnd = rng::random_float() * 0x00ffffff;
  return Color(0xff000000 | rnd);
}

Color operator+(const Color& color, const glm::vec3& other)
{
  return Color(color.value.r + other.r, color.value.g + other.g, color.value.b + other.b, color.value.a);
}

Color operator-(const Color& color, const glm::vec3& other)
{
  return Color(color.value.r - other.r, color.value.g - other.g, color.value.b - other.b, color.value.a);
}

Color operator*(const Color& color, const glm::vec3& other)
{
  return Color(color.value.r * other.r, color.value.g * other.g, color.value.b * other.b, color.value.a);
}

Color operator/(const Color& color, const glm::vec3& other)
{
  return Color(color.value.r / other.r, color.value.g / other.g, color.value.b / other.b, color.value.a);
}

Color operator+(const Color& a, const Color& b)
{
  return Color(a.value + b.value);
}

Color operator-(const Color& a, const Color& b)
{
  return Color(a.value - b.value);
}

Color operator*(const Color& a, const Color& b)
{
  return Color(a.value * b.value);
}

Color operator/(const Color& a, const Color& b)
{
  return Color(a.value / b.value);
}
