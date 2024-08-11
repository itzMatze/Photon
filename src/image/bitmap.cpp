#include "image/bitmap.hpp"

Bitmap::Bitmap(const std::vector<Color>& pixels, glm::uvec2 resolution) : resolution(resolution)
{
  for (const auto& color : pixels) this->pixels.emplace_back(color.get_hex_color());
}

Bitmap::Bitmap(const std::vector<Color>& pixels, uint32_t width, uint32_t height) : resolution(width, height)
{
  for (const auto& color : pixels) this->pixels.emplace_back(color.get_hex_color());
}

Bitmap::Bitmap(const std::vector<uint32_t>& pixels, glm::uvec2 resolution) : pixels(pixels), resolution(resolution)
{}

Bitmap::Bitmap(const std::vector<uint32_t>& pixels, uint32_t width, uint32_t height) : pixels(pixels), resolution(width, height)
{}

Color Bitmap::get(uint32_t x, uint32_t y) const
{
  return Color(pixels[y * resolution.x + x]);
}

const std::vector<uint32_t>& Bitmap::get_hex_vector() const
{
  return pixels;
}

glm::uvec2 Bitmap::get_resolution() const
{
  return resolution;
}
