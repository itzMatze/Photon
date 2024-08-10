#pragma once

#include <vector>
#include "SDL2/SDL_surface.h"
#include "util/named_bitfield.hpp"
#include "renderer/color.hpp"
#include "util/vec2.hpp"

enum class OutputTargetFlags : uint32_t
{
  None = 0,
  ColorArray = (1 << 0),
  SDLSurface = (1 << 1),
};

using OutputTargets = NamedBitfield<OutputTargetFlags>;
ENABLE_ENUM_OPERATORS(OutputTargetFlags);

class Output
{
public:
  Output() = default;
  void init(glm::uvec2 resolution, OutputTargets targets = OutputTargetFlags::ColorArray);
  void destroy();
  void clear();
  void set_pixel(glm::uvec2 pixel, const Color& color);
  void set_pixel(uint32_t x, uint32_t y, const Color& color);
  void set_pixel(uint32_t idx, const Color& color);
  const std::vector<Color>& get_pixels() const;
  SDL_Surface* get_sdl_surface() const;

private:
  bool initialized = false;
  glm::uvec2 resolution;
  OutputTargets targets;
  std::vector<Color> pixels;
  SDL_Surface* sdl_surface = nullptr;
};
