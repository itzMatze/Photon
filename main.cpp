#include <iostream>

#include "render/Renderer.h"

void game_loop(Renderer renderer)
{
    while (1)
    {
        renderer.render_frame();
        SDL_Event e;
        if (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                return;
            }
            else if (e.type == SDL_MOUSEMOTION)
            {
                int x = e.motion.x;
                int y = e.motion.y;
                std::cout << "x: " << x << ", y: " << y << std::endl;
                renderer.set_pixel(x, y, Color(1.0f, 0.0f, 0.0f, 1.0f));
            }
        }
    }
}

int main()
{

    int width = 1920;
    int height = 1080;
    Renderer renderer(width, height);
    // Ray Tracer
    game_loop(renderer);
    return 0;
}
