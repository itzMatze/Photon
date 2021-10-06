#include "Renderer.h"

#define imgui_height 500
#define imgui_width 400

Renderer::Renderer(int width, int render_width, int render_height) : render_width(render_width),
                                                                     render_height(render_height)
{
    SDL_Init(SDL_INIT_EVERYTHING);

    win = SDL_CreateWindow("Ray tracing", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width,
                           float(width) * float(render_height) / float(render_width),
                           SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);

    sdl_renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    SDL_RenderSetLogicalSize(sdl_renderer, render_width, render_height);

    bitmap_surface = SDL_CreateRGBSurface(0, render_width, render_height, 32,
                                          0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
    clean_surface(Color(0.0f, 0.0f, 0.0f, 0.0f));

    imgui_win = SDL_CreateWindow("ImGui", 0, 300, imgui_width,
                                 imgui_height, SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALLOW_HIGHDPI);
    imgui_sdl_renderer = SDL_CreateRenderer(imgui_win, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(imgui_win);
    ImGui_ImplSDLRenderer_Init(imgui_sdl_renderer);
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

void Renderer::render_frame(bool show_gui, int& scene_index, bool& incremental, bool& save)
{
    SDL_RenderClear(sdl_renderer);
    if (show_gui)
    {
        if (imgui_win == nullptr)
        {
            imgui_win = SDL_CreateWindow("ImGui", 0, 300, imgui_width,
                                         imgui_height,
                                         SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALLOW_HIGHDPI);
            imgui_sdl_renderer = SDL_CreateRenderer(imgui_win, -1,
                                                    SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
            ImGui_ImplSDL2_InitForSDLRenderer(imgui_win);
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
        ImGui::Text("Scene:");
        ImGui::SameLine();
        if (ImGui::Button("1"))
        {
            scene_index = 1;
        }
        ImGui::SameLine();
        if (ImGui::Button("2"))
        {
            scene_index = 2;
        }
        ImGui::SameLine();
        if (ImGui::Button("3"))
        {
            incremental = !incremental;
            scene_index = 0;
        }
        if (ImGui::Button("Save Image"))
        {
            save = true;
        }
        ImGui::End();
        ImGui::Render();
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(imgui_sdl_renderer);
    }
    else if (imgui_win != nullptr)
    {
        SDL_DestroyWindow(imgui_win);
        SDL_DestroyRenderer(imgui_sdl_renderer);
        imgui_sdl_renderer = nullptr;
        imgui_win = nullptr;
    }
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
