#include "Renderer.h"

Renderer::Renderer(int width, int render_width, int render_height) : render_width(render_width)
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

    win = SDL_CreateWindow("Ray tracing", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, float(width) * float(render_height) / float(render_width), 0);//SDL_WINDOW_FULLSCREEN_DESKTOP);

    sdl_renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderSetLogicalSize(sdl_renderer, render_width, render_height);
    SDL_GL_SetSwapInterval(1);

    bitmap_surface = SDL_CreateRGBSurface(0, render_width, render_height, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
    for (int i = 0; i < render_width; ++i)
    {
        for (int j = 0; j < render_height; ++j)
        {
            set_pixel(i, j, Color(0.0f, 0.0f, 0.0f, 0.0f));
        }
    }
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
    //assert(x < width && y < height);
    //SDL_LockSurface(bitmap_surface);
    ((unsigned int*) (bitmap_surface->pixels))[(x + render_width * y)] = color.get_hex_color();
    //SDL_UnlockSurface(bitmap_surface);
}

const void* Renderer::get_pixels()
{
    return bitmap_surface->pixels;
}
