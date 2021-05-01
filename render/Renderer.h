#ifndef RAY_TRACER_CPP_RENDERER_H
#define RAY_TRACER_CPP_RENDERER_H

#include <vector>
#include <glew.h>
#define SDL_MAIN_HANDLED
#include <SDL.h>

#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")

#include "VertexBuffer.h"
#include "Shader.h"
#include "Buffer.h"

class Renderer
{
public:
    Renderer() = default;
    void init(int width, int height);
    void update_texture();
    void render_frame();
    void set_pixel(int i, int j, Color color);
    Color get_pixel(int i, int j) const;
    int get_width() const;
    int get_height() const;
private:
    void set_up_sdl();
    int set_up_opengl();

    SDL_Window* window;
    VertexBuffer vertex_buffer;
    Shader shader;
    Buffer buffer;
    GLuint texture_id;
};

#endif //RAY_TRACER_CPP_RENDERER_H
