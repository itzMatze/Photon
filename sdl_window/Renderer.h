#ifndef RAY_TRACING_IOW_RENDERER_H
#define RAY_TRACING_IOW_RENDERER_H

#include <iostream>
#include <imgui.h>
#include <backends/imgui_impl_sdl.h>
#include <backends/imgui_impl_sdlrenderer.h>
#define SDL_MAIN_HANDLED
#define SDL_HAVE_RENDER_GEOMETRY 1
#include <SDL.h>

#include "Color.h"

class Renderer
{
public:
    Renderer(int width, int render_width, int render_height);
    ~Renderer();
    void clean_up_sdl() const;
    void clean_surface(Color color);
    void render_frame(bool show_gui, int& scene_index, bool& incremental, bool& save);
    inline void set_pixel(int x, int y, Color color);
    const void* get_pixels();
    Color get_pixel(int x, int y) const;

    int render_width;
    int render_height;
private:
    SDL_Window* win = nullptr;
    SDL_Window* imgui_win = nullptr;
    SDL_Renderer* sdl_renderer = nullptr;
    SDL_Renderer* imgui_sdl_renderer = nullptr;
    SDL_Surface* bitmap_surface = nullptr;
};

#endif //RAY_TRACING_IOW_RENDERER_H
