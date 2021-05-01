#include <iostream>
#include <vector>

#include "defines.h"
#include "render/Shader.h"
#include "render/VertexBuffer.h"
#include "render/Buffer.h"
#include "render/Renderer.h"

static Renderer renderer;

void game_loop()
{
    SDL_GL_SetSwapInterval(0);
    bool close = false;
    while(!close) {
        renderer.render_frame();
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                close = true;
            }
            else if(event.type == SDL_MOUSEMOTION)
            {
                int x = event.motion.x;
                int y = renderer.get_height() - event.motion.y;
                std::cout << "x: " << x << ", y: " << y << std::endl;
                renderer.set_pixel(x, y, Color{255, 0, 0, 255});
            }
        }
        renderer.update_texture();
    }
}

int main()
{
#if 0
    SDL_Window *win = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Texture *bitmapTex = nullptr;
    SDL_Surface *bitmapSurface = nullptr;
    int width = 1920, height = 1080;

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

    win = SDL_CreateWindow("Hello World", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);

    renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    bitmapSurface = SDL_LoadBMP("img/hello.bmp");
    bitmapTex = SDL_CreateTextureFromSurface(renderer, bitmapSurface);
    SDL_FreeSurface(bitmapSurface);

    while (1) {
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                break;
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, bitmapTex, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(bitmapTex);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);

    SDL_Quit();

    return 0;
#else
    renderer.init(1920, 1080);
    // TODO call Ray Tracer here to fill texture
    game_loop();
    return 0;
#endif
}
