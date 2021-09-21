#include "Renderer.h"

Renderer::Renderer(int width, int render_width, int render_height) : render_width(render_width),
                                                                     render_height(render_height)
{
    SDL_Init(SDL_INIT_EVERYTHING);

    win = SDL_CreateWindow("Ray tracing", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width,
                           float(width) * float(render_height) / float(render_width), 0);

    sdl_renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderSetLogicalSize(sdl_renderer, render_width, render_height);
    SDL_GL_SetSwapInterval(1);

    bitmap_surface = SDL_CreateRGBSurface(0, render_width, render_height, 32,
                                          0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
    clean_surface(Color(0.0f, 0.0f, 0.0f, 0.0f));
}

Renderer::~Renderer()
{
    // sdl seems to clean itself up
    //clean_up_sdl();
}

void Renderer::clean_up_sdl() const
{
    SDL_FreeSurface(bitmap_surface);
    SDL_DestroyRenderer(sdl_renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();
}

void Renderer::clean_surface(Color color)
{
    for (int i = 0; i < render_width; ++i)
    {
        for (int j = 0; j < render_height; ++j)
        {
            set_pixel(i, j, color);
        }
    }
}

void Renderer::render_frame()
{
    SDL_RenderClear(sdl_renderer);
    SDL_Texture* bitmapTex = SDL_CreateTextureFromSurface(sdl_renderer, bitmap_surface);
    SDL_RenderCopy(sdl_renderer, bitmapTex, nullptr, nullptr);
    SDL_RenderPresent(sdl_renderer);
    SDL_DestroyTexture(bitmapTex);
}

inline void Renderer::set_pixel(int x, int y, Color color)
{
    assert(x < render_width && y < render_height);
    //SDL_LockSurface(bitmap_surface);
    ((uint32_t*) (bitmap_surface->pixels))[(x + render_width * y)] = color.get_hex_color();
    //SDL_UnlockSurface(bitmap_surface);
}

const void* Renderer::get_pixels()
{
    return bitmap_surface->pixels;
}

Color Renderer::get_pixel(int x, int y) const
{
    assert(x < render_width && y < render_height);
    return {((uint32_t*) (bitmap_surface->pixels))[x + render_width * y]};
}
