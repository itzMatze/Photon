#include "Renderer.h"

Renderer::Renderer(int width, int height) : width(width), height(height)
{

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

    win = SDL_CreateWindow("Hello World", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);//SDL_WINDOW_FULLSCREEN_DESKTOP);

    sdl_renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    SDL_GL_SetSwapInterval(1);

    bitmapSurface = SDL_CreateRGBSurface(0, width, height, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
    for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < height; ++j)
        {
            set_pixel(i, j, Color(1.0f, 1.0f, 1.0f, 1.0f));
        }
    }
}

Renderer::~Renderer()
{
    //clean_up_sdl();
}

void Renderer::clean_up_sdl() const
{
    SDL_FreeSurface(bitmapSurface);
    SDL_DestroyRenderer(sdl_renderer);
    SDL_DestroyWindow(win);

    SDL_Quit();
}

void Renderer::set_pixel(int x, int y, Color color)
{
    SDL_LockSurface(bitmapSurface);
    ((unsigned int*) (bitmapSurface->pixels))[(x + width * y)] = color.get_hex_color();
    SDL_UnlockSurface(bitmapSurface);
}

void Renderer::render_frame()
{
    SDL_RenderClear(sdl_renderer);
    SDL_Texture* bitmapTex = SDL_CreateTextureFromSurface(sdl_renderer, bitmapSurface);
    SDL_RenderCopy(sdl_renderer, bitmapTex, nullptr, nullptr);
    SDL_RenderPresent(sdl_renderer);
    SDL_DestroyTexture(bitmapTex);
}