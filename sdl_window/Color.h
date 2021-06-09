#ifndef RAY_TRACING_IOW_COLOR_H
#define RAY_TRACING_IOW_COLOR_H

#include <vec4.hpp>

class Color
{
public:
    Color();
    Color(float r, float g, float b);
    Color(float r, float g, float b, float a);
    unsigned int get_hex_color() const;

    glm::vec4 values;
};

#endif //RAY_TRACING_IOW_COLOR_H
