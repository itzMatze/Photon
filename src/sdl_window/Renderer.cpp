#include "sdl_window/Renderer.h"

#define imgui_height 500
#define imgui_width 400

Renderer::Renderer(int width, const int render_width, const int render_height, const bool use_surface) : 
render_width(render_width), render_height(render_height), use_surface(use_surface)
{
    surface = std::vector<std::vector<glm::vec4>>(render_width, std::vector<glm::vec4>(render_height, glm::vec4(0.0f)));

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
                                 imgui_height, SDL_WINDOW_OPENGL /*| SDL_WINDOW_BORDERLESS*/ | SDL_WINDOW_ALLOW_HIGHDPI);
    imgui_sdl_renderer = SDL_CreateRenderer(imgui_win, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(imgui_win, imgui_sdl_renderer);
    ImGui_ImplSDLRenderer_Init(imgui_sdl_renderer);
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
    bitmap_surface = nullptr;
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
        if (ImGui::Button("Save Image"))
        {
            save = true;
        }
        ImGui::Text("Press 'G' to show or hide this GUI-Window");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);
        ImGui::Separator();
        ImGui::Text("Scene:");
        ImGui::SameLine();
        if (ImGui::RadioButton("1", r_info.scene_index == 1))
        {
            r_info.scene_index = 1;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("2", r_info.scene_index == 2))
        {
            r_info.scene_index = 2;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("3", r_info.scene_index == 3))
        {
            r_info.scene_index = 3;
        }
        if (ImGui::Button(r_info.incremental ? "Render incrementally: true" : "Render incrementally: false"))
        {
            r_info.incremental = !r_info.incremental;
        }
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 100, 0, 255));
        if (ImGui::Button("Apply"))
        {
            apply = true;
        }
        ImGui::PopStyleColor();
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
    SDL_Texture* bitmapTex = SDL_CreateTextureFromSurface(sdl_renderer, bitmap_surface);
    SDL_RenderCopy(sdl_renderer, bitmapTex, nullptr, nullptr);
    SDL_RenderPresent(sdl_renderer);
    SDL_DestroyTexture(bitmapTex);
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
