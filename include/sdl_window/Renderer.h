#pragma once

#include <iostream>
#include <vector>
#include <chrono>
#include <imgui.h>
#include <backends/imgui_impl_sdl.h>
#include <backends/imgui_impl_sdlrenderer.h>

#define SDL_MAIN_HANDLED
#include "SDL.h"

#include "Color.h"

enum GUIChange
{
    NO_CHANGES = (0u),
    CAM_CHANGE = (1u << 0),
    RESOLUTION_CHANGE = (1u << 1),
    SCENE_CHANGE = (1u << 2),
    OTHER_CHANGE = (1u << 31)
};

struct RenderingInfo
{
    RenderingInfo() = default;
    RenderingInfo(int ns, int nx, int ny, int max_depth, int scene_index, bool incremental, bool spectral, 
        glm::vec3 origin, glm::vec3 look_at, glm::vec3 up, float vfov, float aperture, float focus_dist) :
            changes(NO_CHANGES), ns(ns), nx(nx), ny(ny), max_depth(max_depth), scene_index(scene_index),
            save_on_finish(false), incremental(incremental), spectral(spectral),
            origin(origin), look_at(look_at), up(up), vfov(vfov), aperture(aperture), focus_dist(focus_dist)
    {}
    uint32_t changes;
    int ns;
    int nx;
    int ny;
    int max_depth;
    int scene_index;
    bool save_on_finish;
    bool incremental;
    bool spectral;
    glm::vec3 origin;
    glm::vec3 look_at;
    glm::vec3 up;
    float vfov;
    float aperture;
    float focus_dist;

};

class Renderer
{
public:
    Renderer(int width, const int render_width, const int render_height, const bool use_surface = false);
    void create_window(const uint32_t width, const uint32_t render_width, const uint32_t render_height);
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
    std::chrono::time_point<std::chrono::steady_clock> time = std::chrono::steady_clock::now();
    bool show_gui = true;
    bool use_surface = false;
    SDL_Window* win = nullptr;
    SDL_Window* imgui_win = nullptr;
    SDL_Renderer* sdl_renderer = nullptr;
    SDL_Renderer* imgui_sdl_renderer = nullptr;
    SDL_Surface* bitmap_surface = nullptr;
    std::vector<std::vector<glm::vec4>> surface;
};
