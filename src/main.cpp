#include "RayTracer.h"
// choose to render image fast or in great quality

bool handle_event(SDL_Event* e, Renderer& render_window, RayTracer& tracer)
{
    if (e->window.event == SDL_WINDOWEVENT_CLOSE)
    {
        tracer.stop();
        return true;
    }
    switch (e->type)
    {
        case SDL_KEYDOWN:
            switch (e->key.keysym.sym)
            {
                case SDLK_g:
                    render_window.toggle_show_gui();
                    break;
                default:
                    break;
            }
            break;
    }
    return false;
}

int main()
{
#if 1
    constexpr int nx = 800;
    constexpr int ny = 500;
    RenderingInfo r_info{1024, 6, 2, true, true};
#else
    constexpr int nx = 3840;
    constexpr int ny = 2160;
    RenderingInfo r_info{4096, 8, 1, true, true};
#endif
    Camera cam(glm::vec3(0.0f, 1.5f, 0.0f), glm::vec3(0.0f, 0.0f, -15.0f), glm::vec3(0.0f, 1.0f, 0.0f),
               90.0f, float(nx) / float(ny), 0.0001f, 2.0f);
    Renderer render_window(1000, nx, ny);
    // TODO render first in lower resolution and then add resolution (this is probably not so easy)
    RayTracer tracer(cam, render_window);
    tracer.trace(r_info);
    // render loop
    bool save = false;
    bool quit = false;
    while (!quit)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            ImGui_ImplSDL2_ProcessEvent(&e);
            quit = handle_event(&e, render_window, tracer);
        }
        if (render_window.render_frame(r_info, save))
        {
            tracer.stop();
            tracer.trace(r_info);
        }
        if (save)
        {
            save_image((uint32_t*) render_window.get_pixels(), "", nx, ny, 4);
            std::cout << "Image saved!" << std::endl;
            save = false;
        }
        std::this_thread::yield();
    }
    return 0;
}
