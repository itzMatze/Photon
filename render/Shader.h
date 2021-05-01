#pragma once
#include <glew.h>
#include <string>

struct Shader {
    Shader() = default;
    void init(const char* vertexShaderFilename, const char* fragmentShaderFilename);
    virtual ~Shader();

    void bind() const;
    void unbind();

    GLuint getShaderId() {
        return shaderId;
    }

private:

    GLuint compile(const std::string& shaderSource, GLenum type);
    std::string parse(const char* filename);
    GLuint create_shader(const char* vertexShaderFilename, const char* fragmentShaderFilename);

    GLuint shaderId{};
};