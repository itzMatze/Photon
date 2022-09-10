#include "sdl_window/Renderer.h"

#include "glm/gtc/type_ptr.hpp"

#define imgui_height 500
#define imgui_width 400

Renderer::Renderer(int width, const int render_width, const int render_height, const bool use_surface) : use_surface(use_surface)
{
    SDL_Init(SDL_INIT_VIDEO);

    create_window(width, render_width, render_height);

    imgui_win = SDL_CreateWindow("ImGui", 0, 300, imgui_width,
                                 imgui_height, SDL_WINDOW_OPENGL /*| SDL_WINDOW_BORDERLESS*/ | SDL_WINDOW_ALLOW_HIGHDPI);
    imgui_sdl_renderer = SDL_CreateRenderer(imgui_win, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(imgui_win, imgui_sdl_renderer);
    ImGui_ImplSDLRenderer_Init(imgui_sdl_renderer);
}

void Renderer::create_window(const uint32_t width, const uint32_t render_w, const uint32_t render_h)
{
    render_width = render_w;
    render_height = render_h;

    surface = std::vector<std::vector<glm::vec4>>(render_width, std::vector<glm::vec4>(render_height, glm::vec4(0.0f)));

    if (bitmap_surface != nullptr)
    {
        SDL_FreeSurface(bitmap_surface);
        bitmap_surface = nullptr;
    }
    if (sdl_renderer != nullptr)
    {
        SDL_DestroyRenderer(sdl_renderer);
        sdl_renderer = nullptr;
    }
    if (win != nullptr)
    {
        SDL_DestroyWindow(win);
        win = nullptr;
    }
    win = SDL_CreateWindow("Ray tracing", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width,
                           float(width) * float(render_height) / float(render_width),
                           SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);

    sdl_renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    SDL_RenderSetLogicalSize(sdl_renderer, render_width, render_height);

    bitmap_surface = SDL_CreateRGBSurface(0, render_width, render_height, 32,
                                          0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
    clean_surface(Color(0.0f, 0.0f, 0.0f, 0.0f));
}

Renderer::~Renderer()
{
    // sdl seems to clean itself up
    clean_up_sdl();
}

void Renderer::clean_up_sdl()
{
    SDL_FreeSurface(bitmap_surface);
    bitmap_surface = nullptr;
    SDL_DestroyRenderer(sdl_renderer);
    sdl_renderer = nullptr;
    SDL_DestroyRenderer(imgui_sdl_renderer);
    imgui_sdl_renderer = nullptr;
    SDL_DestroyWindow(win);
    win = nullptr;
    SDL_DestroyWindow(imgui_win);
    imgui_win = nullptr;
    SDL_Quit();
}

void Renderer::clean_surface(Color color)
{
    for (int i = 0; i < render_width; ++i)
    {
        for (int j = 0; j < render_height; ++j)
        {
            set_pixel(i, j, color);
            if (use_surface) surface[i][j] = color.values;
        }
    }
}

bool Renderer::render_frame(RenderingInfo& r_info, bool& save)
{
    bool apply = false;
    SDL_RenderClear(sdl_renderer);
    if (show_gui)
    {
        if (imgui_win == nullptr)
        {
            imgui_win = SDL_CreateWindow("ImGui", 0, 300, imgui_width,
                                         imgui_height,
                                         SDL_WINDOW_OPENGL /*| SDL_WINDOW_BORDERLESS*/ | SDL_WINDOW_ALLOW_HIGHDPI);
            imgui_sdl_renderer = SDL_CreateRenderer(imgui_win, -1,
                                                    SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
            ImGui_ImplSDL2_InitForSDLRenderer(imgui_win, imgui_sdl_renderer);
            ImGui_ImplSDLRenderer_Init(imgui_sdl_renderer);
        }
        SDL_RenderClear(imgui_sdl_renderer);
        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame(win);
        ImGui::NewFrame();
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(imgui_width, imgui_height));
        ImGui::Begin("ImGui");
        ImGui::Text("Press 'G' to show or hide this GUI-Window");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);
        ImGui::Separator();
        ImGui::Text("Scene:");
        ImGui::SameLine();
        if (ImGui::RadioButton("1", r_info.scene_index == 1))
        {
            r_info.scene_index = 1;
            r_info.changes |= SCENE_CHANGE;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("2", r_info.scene_index == 2))
        {
            r_info.scene_index = 2;
            r_info.changes |= SCENE_CHANGE;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("3", r_info.scene_index == 3))
        {
            r_info.scene_index = 3;
            r_info.changes |= SCENE_CHANGE;
        }
        ImGui::Text("Resolution:");
        ImGui::SameLine();
        if (ImGui::RadioButton("Preview", r_info.nx == 800))
        {
            r_info.nx = 800;
            r_info.ny = 500;
            r_info.changes |= RESOLUTION_CHANGE;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("FullHD", r_info.nx == 1920))
        {
            r_info.nx = 1920;
            r_info.ny = 1200;
            r_info.changes |= RESOLUTION_CHANGE;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("4K", r_info.nx == 3840))
        {
            r_info.nx = 3840;
            r_info.ny = 2400;
            r_info.changes |= RESOLUTION_CHANGE;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("5K", r_info.nx == 5120))
        {
            r_info.nx = 5120;
            r_info.ny = 3200;
            r_info.changes |= RESOLUTION_CHANGE;
        }
        if (ImGui::InputInt("Sample count", &r_info.ns, 0))
        {
            r_info.changes |= RESOLUTION_CHANGE;
        }
        if (ImGui::InputInt("Max depth", &r_info.max_depth));
        ImGui::Separator();
        ImGui::Text("Camera");
        if (ImGui::InputFloat3("Origin", glm::value_ptr(r_info.origin))) r_info.changes |= CAM_CHANGE;
        if (ImGui::InputFloat3("Look at", glm::value_ptr(r_info.look_at))) r_info.changes |= CAM_CHANGE;
        if (ImGui::InputFloat3("Up", glm::value_ptr(r_info.up))) r_info.changes |= CAM_CHANGE;
        if (ImGui::InputFloat("Vertical FOV", &r_info.vfov, 0.0f) | ImGui::InputFloat("Aperture", &r_info.aperture, 0.0f, 0.0f, "%.9f") | ImGui::InputFloat("Focus distance", &r_info.focus_dist, 0.0f))
        {
            r_info.changes |= CAM_CHANGE;
        }
        if (ImGui::Button(r_info.incremental ? "incrementally: true" : "incrementally: false"))
        {
            r_info.incremental = !r_info.incremental;
        }
        if (ImGui::Button(r_info.spectral ? "spectral: true" : "spectral: false"))
        {
            r_info.spectral = !r_info.spectral;
        }
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 100, 0, 255));
        if (ImGui::Button("Apply"))
        {
            apply = true;
        }
        ImGui::PopStyleColor();
        ImGui::SameLine();
        if (ImGui::Button(r_info.save_on_finish ? "Saving image when done!" : "Not saving image!"))
        {
            r_info.save_on_finish = !r_info.save_on_finish;
        }
        ImGui::SameLine();
        if (ImGui::Button("Save Image now"))
        {
            save = true;
        }
        ImGui::Separator();
        ImGui::End();
        ImGui::Render();
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(imgui_sdl_renderer);
    }
        // user doesn't want gui but it is still there, so destroy it
    else if (imgui_win != nullptr)
    {
        SDL_DestroyWindow(imgui_win);
        imgui_win = nullptr;
        SDL_DestroyRenderer(imgui_sdl_renderer);
        imgui_sdl_renderer = nullptr;
    }
    // refresh the screen only every 1 seconds
    std::chrono::duration<double, std::milli> x = std::chrono::steady_clock::now() - time;
    if (x.count() > 1000.0)
    {
        SDL_Texture* bitmapTex = SDL_CreateTextureFromSurface(sdl_renderer, bitmap_surface);
        SDL_RenderCopy(sdl_renderer, bitmapTex, nullptr, nullptr);
        SDL_RenderPresent(sdl_renderer);
        SDL_DestroyTexture(bitmapTex);
        time = std::chrono::steady_clock::now();
    }
    return apply;
}

const void* Renderer::get_pixels()
{
    return bitmap_surface->pixels;
}

glm::vec4 Renderer::get_pixel(int x, int y) const
{
    assert(x < render_width && y < render_height);
    return use_surface ? surface[x][y] : Color(((uint32_t*) (bitmap_surface->pixels))[x + render_width * y]).values;
}
