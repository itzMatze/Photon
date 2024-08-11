#pragma once

#include <cstdint>
#include <vector>
#include "renderer/color.hpp"
#include "util/vec2.hpp"

class Bitmap
{
public:
  Bitmap(const std::vector<Color>& pixels, glm::uvec2 resolution);
  Bitmap(const std::vector<Color>& pixels, uint32_t width, uint32_t height);
  Bitmap(const std::vector<uint32_t>& pixels, glm::uvec2 resolution);
  Bitmap(const std::vector<uint32_t>& pixels, uint32_t width, uint32_t height);
  Color get(uint32_t x, uint32_t y) const;
  const std::vector<uint32_t>& get_hex_vector() const;
  glm::uvec2 get_resolution() const;

private:
  std::vector<uint32_t> pixels;
  glm::uvec2 resolution;
};
