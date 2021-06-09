#ifndef RAY_TRACING_IOW_RENDERER_H
#define RAY_TRACING_IOW_RENDERER_H

#include <iostream>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <cassert>

#include "Color.h"

class Renderer
{
public:
    Renderer(int width, int render_width, int render_height);
    ~Renderer();
    void clean_up_sdl() const;
    void render_frame();
    inline void set_pixel(int x, int y, Color color);
    const void* get_pixels();

    int render_width;
private:
    SDL_Window* win = nullptr;
    SDL_Renderer* sdl_renderer = nullptr;
    SDL_Surface* bitmap_surface = nullptr;
};

#endif //RAY_TRACING_IOW_RENDERER_H
