#pragma once

#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "util/vec2.hpp"

class Window
{
public:
  Window(int width, glm::uvec2 render_resolution);
  ~Window();
  void update_content(SDL_Surface* bitmap_surface);
  bool get_inputs();

private:
  glm::uvec2 render_resolution;
  SDL_Window* win = nullptr;
  SDL_Renderer* sdl_renderer = nullptr;
  SDL_Texture* bitmap_texture = nullptr;
};
