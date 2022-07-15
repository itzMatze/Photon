#pragma once

#include <iostream>
#include <vector>
#include <imgui.h>
#include <backends/imgui_impl_sdl.h>
#include <backends/imgui_impl_sdlrenderer.h>

#define SDL_MAIN_HANDLED
#include "SDL.h"

#include "Color.h"

struct RenderingInfo
{
    int ns;
    int max_depth;
    int scene_index;
    bool incremental;
};

class Renderer
{
public:
    Renderer(int width, const int render_width, const int render_height, const bool use_surface = false);
    ~Renderer();
    void clean_up_sdl();
    void clean_surface(Color color);
    bool render_frame(RenderingInfo& r_info, bool& save);
    inline void set_pixel(int x, int y, Color color)
    {
    assert(x < render_width && y < render_height);
    //SDL_LockSurface(bitmap_surface);
    ((uint32_t*) (bitmap_surface->pixels))[(x + render_width * y)] = color.get_hex_color();
    if (use_surface) surface[x][y] = color.values;
    //SDL_UnlockSurface(bitmap_surface);
    }
    const void* get_pixels();
    glm::vec4 get_pixel(int x, int y) const;

    void toggle_show_gui()
    {show_gui = !show_gui;}

    void set_use_surface(const bool b)
    {use_surface = b;}

    int render_width;
    int render_height;
private:
    bool show_gui = true;
    bool use_surface = false;
    SDL_Window* win = nullptr;
    SDL_Window* imgui_win = nullptr;
    SDL_Renderer* sdl_renderer = nullptr;
    SDL_Renderer* imgui_sdl_renderer = nullptr;
    SDL_Surface* bitmap_surface = nullptr;
    std::vector<std::vector<glm::vec4>> surface;
};
