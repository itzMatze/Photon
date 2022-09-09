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
    constexpr uint32_t num_threads = 15;
    // ns, nx, ny, depth, scene, incremental, spectral, origin, look_at, up, vfov, aperture, focus_dist
    RenderingInfo r_info(4096, 800, 500, 8, 2, true, true, glm::vec3(0.0f, 5.0f, 10.0f), glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 1.0f, 0.0f), 15.0f, 0.0001f, 3.0f);
    Renderer render_window(1000, r_info.nx, r_info.ny, true);
    RayTracer tracer(&render_window, num_threads, r_info);
    tracer.trace();
    // render loop
    bool save = false, quit = false;
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
            tracer.update_render_info(r_info);
            if (r_info.changes & SCENE_CHANGE) tracer.load_scene();
            if (r_info.changes & CAM_CHANGE) tracer.update_cam();
            if (r_info.changes & RESOLUTION_CHANGE) 
            {
                render_window.create_window(1000, r_info.nx, r_info.ny);
                tracer.update_render_window(&render_window);
            }
            r_info.changes = NO_CHANGES;
            tracer.trace();
        }
        if (save)
        {
            save_image((uint32_t*) render_window.get_pixels(), "", r_info.nx, r_info.ny, 4);
            std::cout << "Image saved!" << std::endl;
            save = false;
        }
        if (tracer.done())
        {
            tracer.stop();
            // save must be false at this point, either it was already or it was set above
            save = r_info.save_on_finish;
            r_info.save_on_finish = false;
        }
        std::this_thread::yield();
    }
    tracer.stop();
    return 0;
}
