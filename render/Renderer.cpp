#include <iostream>
#include "Renderer.h"

void Renderer::init(int width, int height)
{
    buffer = Buffer();
    buffer.init(width, height);
    set_up_sdl();
    set_up_opengl();
    std::vector<Vertex> vertices = {
            Vertex{glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)},
            Vertex{glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)},
            Vertex{glm::vec3(1.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)},
            Vertex{glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f)},
            Vertex{glm::vec3(1.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f)},
            Vertex{glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f)}
    };
    vertex_buffer = VertexBuffer();
    vertex_buffer.init(vertices.data(), vertices.size());
    shader = Shader();
    shader.init("../shaders/basic.vert", "../shaders/basic.frag");
}

void Renderer::set_up_sdl()
{
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    window = SDL_CreateWindow("C++ Ray Tracer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, buffer.get_width(), buffer.get_height(), SDL_WINDOW_OPENGL);// | SDL_WINDOW_FULLSCREEN_DESKTOP);
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
}

int Renderer::set_up_opengl()
{
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cout << "Error: " << glewGetErrorString(err) << std::endl;
        std::cin.get();
        return -1;
    }
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, buffer.get_width(), buffer.get_height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, (const void*) buffer.get_texture());
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::render_frame()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    vertex_buffer.bind();
    shader.bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glDrawArrays(GL_TRIANGLES, 0, vertex_buffer.size);

    SDL_GL_SwapWindow(window);
}

void Renderer::update_texture()
{
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, buffer.get_width(), buffer.get_height(), GL_RGBA, GL_UNSIGNED_BYTE, (const void*) buffer.get_texture());
}

void Renderer::set_pixel(int i, int j, Color color)
{
    int width = buffer.get_width();
    int height = buffer.get_height();
    i %= width;
    j %= height;
    i = i < 0 ? 0 : i;
    j = j < 0 ? 0 : j;
    assert(i < width && i >= 0);
    assert(j < height && j >= 0);
    buffer.set_pixel(i, j, color);
}

Color Renderer::get_pixel(int i, int j) const
{
    return buffer.get_pixel(i, j);
}

int Renderer::get_width() const
{
    return buffer.get_width();
}

int Renderer::get_height() const
{
    return buffer.get_height();
}
