#pragma once
#include <glew.h>
#include "defines.h"

struct VertexBuffer {
    VertexBuffer() = default;
    void init(void* data, int num_vertices);
    virtual ~VertexBuffer();
    void bind() const;
    void unbind();

    int size;

private:
    GLuint bufferId{};
    GLuint vao{};
};