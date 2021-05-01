//
// Created by matth on 01.05.2021.
//

#ifndef RAY_TRACER_CPP_RENDERER_H
#define RAY_TRACER_CPP_RENDERER_H

#include <iostream>
#define SDL_MAIN_HANDLED
#include <SDL.h>

#include "Color.h"

class Renderer
{
public:
    Renderer(int width, int height);
    ~Renderer();
    void clean_up_sdl() const;
    void set_pixel(int x, int y, Color color);
    void render_frame();
    int width, height;
private:
    SDL_Window *win = nullptr;
    SDL_Renderer *sdl_renderer = nullptr;
    SDL_Surface *bitmapSurface = nullptr;
};

#endif //RAY_TRACER_CPP_RENDERER_H
