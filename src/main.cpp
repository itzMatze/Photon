#include "RayTracer.h"

class MainContext
{
public:
    MainContext(RenderingInfo& _r_info) : r_info(_r_info), tracer(_r_info.nx, _r_info.ny, 16, _r_info)
    {
        tracer.trace();
    }

    ~MainContext()
    {
        tracer.stop();
    }

    void render_frame()
    {
        if (tracer.get_renderer()->render_frame(r_info, save))
        {
            apply_changes();
        }
        if (save)
        {
            save_image((uint32_t*) tracer.get_renderer()->get_pixels(), "", r_info.nx, r_info.ny, 4);
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
    }

    void apply_changes()
    {
        tracer.stop();
        tracer.update_render_info(r_info);
        if (r_info.changes & SCENE_CHANGE) tracer.load_scene();
        if (r_info.changes & CAM_CHANGE) tracer.update_cam();
        if (r_info.changes & RESOLUTION_CHANGE) 
        {
            tracer.update_renderer(r_info.nx, r_info.ny);            
        }
        r_info.changes = NO_CHANGES;
        tracer.trace();
    }
    
    bool handle_event(SDL_Event* e)
    {
        if (e->window.event == SDL_WINDOWEVENT_CLOSE)
        {
            return true;
        }
        switch (e->type)
        {
            case SDL_KEYDOWN:
                switch (e->key.keysym.sym)
                {
                    case SDLK_g:
                        tracer.get_renderer()->toggle_show_gui();
                        break;
                    case SDLK_i:
                        r_info.incremental = !r_info.incremental;
                        break;
                    case SDLK_1:
                        r_info.scene_index = 1;
                        r_info.changes |= SCENE_CHANGE;
                        break;
                    case SDLK_2:
                        r_info.scene_index = 2;
                        r_info.changes |= SCENE_CHANGE;
                        break;
                    case SDLK_3:
                        r_info.scene_index = 3;
                        r_info.changes |= SCENE_CHANGE;
                        break;
                    case SDLK_RETURN:
                        apply_changes();
                        break;
                    case SDLK_F1:
                        r_info.nx = 800;
                        r_info.ny = 500;
                        r_info.changes |= RESOLUTION_CHANGE;
                        break;
                    case SDLK_F2:
                        r_info.nx = 1920;
                        r_info.ny = 1200;
                        r_info.changes |= RESOLUTION_CHANGE;
                        break;
                    case SDLK_F4:
                        r_info.nx = 3840;
                        r_info.ny = 2400;
                        r_info.changes |= RESOLUTION_CHANGE;
                        break;
                    case SDLK_F5:
                        r_info.nx = 5120;
                        r_info.ny = 3200;
                        r_info.changes |= RESOLUTION_CHANGE;
                        break;
                    case SDLK_KP_0:
                        r_info.ns = 32;
                        break;
                    case SDLK_KP_1:
                        r_info.ns = 128;
                        break;
                    case SDLK_KP_2:
                        r_info.ns = 512;
                        break;
                    case SDLK_KP_3:
                        r_info.ns = 1024;
                        break;
                    case SDLK_KP_4:
                        r_info.ns = 4096;
                        break;
                    case SDLK_SPACE:
                        r_info.ns = 4096;
                        r_info.nx = 5120;
                        r_info.ny = 3200;
                        r_info.max_depth = 10;
                        r_info.incremental = false;
                        r_info.save_on_finish = true;
                        r_info.changes |= RESOLUTION_CHANGE;
                    default:
                        break;
                }
                break;
        }
        return false;
    }

private:
    bool save;
    RenderingInfo r_info;
    RayTracer tracer;
};

int main()
{
    // ns, nx, ny, depth, scene, incremental, spectral, origin, look_at, up, vfov, aperture, focus_dist
    RenderingInfo r_info(4096, 800, 500, 8, 2, true, true, glm::vec3(0.0f, 5.0f, 10.0f), glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 1.0f, 0.0f), 15.0f, 0.0001f, 3.0f);
    MainContext mc(r_info);
    // render loop
    bool save = false, quit = false;
    while (!quit)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            ImGui_ImplSDL2_ProcessEvent(&e);
            quit = mc.handle_event(&e);
        }
        mc.render_frame();
        std::this_thread::yield();
    }
    return 0;
}
