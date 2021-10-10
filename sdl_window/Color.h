#ifndef RAY_TRACING_IOW_COLOR_H
#define RAY_TRACING_IOW_COLOR_H

#include <glm.hpp>

class Color
{
public:
    Color();
    Color(uint32_t hex_color);
    Color(float r, float g, float b);
    Color(float r, float g, float b, float a);
    uint32_t get_hex_color() const;

    glm::vec4 values{};
};

#endif //RAY_TRACING_IOW_COLOR_H
