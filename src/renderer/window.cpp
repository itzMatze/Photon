#include "renderer/window.hpp"

void Window::init(int width, glm::uvec2 render_resolution)
{
  if (initialized) destroy();
  this->render_resolution = render_resolution;
  SDL_Init(SDL_INIT_VIDEO);
  win = SDL_CreateWindow("Photon", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width,
                         float(width) * float(render_resolution.y) / float(render_resolution.x),
                         SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
  sdl_renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
  SDL_RenderSetLogicalSize(sdl_renderer, render_resolution.x, render_resolution.y);
  initialized = true;
}

void Window::destroy()
{
  SDL_DestroyRenderer(sdl_renderer);
  sdl_renderer = nullptr;
  SDL_DestroyWindow(win);
  win = nullptr;
  SDL_Quit();
  initialized = false;
}

void Window::update_content(SDL_Surface* bitmap_surface)
{
  SDL_RenderClear(sdl_renderer);
  bitmap_texture = SDL_CreateTextureFromSurface(sdl_renderer, bitmap_surface);
  SDL_RenderCopy(sdl_renderer, bitmap_texture, nullptr, nullptr);
  SDL_RenderPresent(sdl_renderer);
  SDL_DestroyTexture(bitmap_texture);
}

bool Window::get_inputs()
{
  SDL_Event e;
  while (SDL_PollEvent(&e))
  {
    if (e.window.event == SDL_WINDOWEVENT_CLOSE) return false;
  }
  return true;
}
