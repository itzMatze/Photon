#include "VertexBuffer.h"

void VertexBuffer::init(void* data, int num_vertices)
{
    size = num_vertices;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &bufferId);
    glBindBuffer(GL_ARRAY_BUFFER, bufferId);
    glBufferData(GL_ARRAY_BUFFER, size * sizeof(Vertex), data, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) (sizeof(float) * 0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) (sizeof(float) * 3));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) (sizeof(float) * 5));
    glBindVertexArray(0);
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &bufferId);
}

void VertexBuffer::bind() const
{
    glBindVertexArray(vao);
}

void VertexBuffer::unbind()
{
    glBindVertexArray(0);
}