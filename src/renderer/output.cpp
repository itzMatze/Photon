#include "renderer/output.hpp"
#include <SDL2/SDL_surface.h>

Output::Output(glm::uvec2 resolution, OutputTargets targets) : resolution(resolution), targets(targets)
{
  if (targets & OutputTargetFlags::ColorArray) pixels = std::vector<Color>(resolution.x * resolution.y);
  sdl_surface = SDL_CreateRGBSurface(0, resolution.x, resolution.y, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
  clear();
}

Output::~Output()
{
  SDL_FreeSurface(sdl_surface);
  sdl_surface = nullptr;
}

void Output::clear()
{
  for (int i = 0; i < resolution.x * resolution.y; ++i)
  {
    set_pixel(i, Color(0));
  }
}

void Output::set_pixel(glm::uvec2 pixel, const Color& color)
{
  assert(pixel.x < resolution.x && pixel.y < resolution.y);
  set_pixel(resolution.x * pixel.y + pixel.x, color);
}

void Output::set_pixel(uint32_t x, uint32_t y, const Color& color)
{
  assert(x < resolution.x && y < resolution.y);
  set_pixel(resolution.x * y + x, color);
}

void Output::set_pixel(uint32_t idx, const Color& color)
{
  assert(idx < resolution.x * resolution.y);
  if (targets & OutputTargetFlags::ColorArray) pixels[idx] = color;
  if (targets & OutputTargetFlags::SDLSurface) ((uint32_t*) (sdl_surface->pixels))[idx] = color.get_hex_color();
}

const std::vector<Color>& Output::get_pixels() const
{
  assert(targets & OutputTargetFlags::ColorArray);
  return pixels;
}

SDL_Surface* Output::get_sdl_surface() const
{
  assert(targets & OutputTargetFlags::SDLSurface);
  return sdl_surface;
}

