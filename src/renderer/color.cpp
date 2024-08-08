#include "renderer/color.hpp"
#include <algorithm>
#include "util/random_generator.hpp"

Color::Color() : value(0.0), alpha(0.0)
{}

Color::Color(uint32_t hex_color)
{
  value.x = float(hex_color & 0x000000ff) / 255.0f;
  hex_color >>= 8;
  value.y = float(hex_color & 0x000000ff) / 255.0f;
  hex_color >>= 8;
  value.z = float(hex_color & 0x000000ff) / 255.0f;
  hex_color >>= 8;
  alpha = float(hex_color) / 255.0f;
}

Color::Color(const cm::Vec3& rgb) : value(rgb), alpha(1.0)
{}

Color::Color(float r, float g, float b) : value(r, g, b), alpha(1.0)
{}

Color::Color(float r, float g, float b, float a) : value(r, g, b), alpha(a)
{}

uint32_t Color::get_hex_color() const
{
  // color values > 1.0 are allowed, but they need to be clamped when the color gets converted to hex representation
  uint32_t hex_color = std::clamp(static_cast<uint64_t>(alpha * 255.999f), uint64_t(0), uint64_t(255));
  hex_color <<= 8;
  hex_color += std::clamp(static_cast<uint64_t>(value.z * 255.999f), uint64_t(0), uint64_t(255));
  hex_color <<= 8;
  hex_color += std::clamp(static_cast<uint64_t>(value.y * 255.999f), uint64_t(0), uint64_t(255));
  hex_color <<= 8;
  hex_color += std::clamp(static_cast<uint64_t>(value.x * 255.999f), uint64_t(0), uint64_t(255));
  return hex_color;
}

Color Color::get_random_color()
{
  uint32_t rnd = rng::random_float() * 0x00ffffff;
  return Color(0xff000000 | rnd);
}

