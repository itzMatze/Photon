#pragma once

#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "util/vec2.hpp"

class Window
{
public:
  Window() = default;
  void init(int width, glm::uvec2 render_resolution);
  void destroy();
  void update_content(SDL_Surface* bitmap_surface);
  bool get_inputs();

private:
  bool initialized = false;
  glm::uvec2 render_resolution;
  SDL_Window* win = nullptr;
  SDL_Renderer* sdl_renderer = nullptr;
  SDL_Texture* bitmap_texture = nullptr;
};
