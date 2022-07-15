#include "sdl_window/Color.h"

Color::Color() : values(glm::vec4(0.0f))
{
}

Color::Color(uint32_t hex_color)
{
    values.r = float(hex_color % 256) / 255.0f;
    hex_color >>= 8;
    values.g = float(hex_color % 256) / 255.0f;
    hex_color >>= 8;
    values.b = float(hex_color % 256) / 255.0f;
    hex_color >>= 8;
    values.a = float(hex_color) / 255.0f;
}

Color::Color(float r, float g, float b) : values(glm::vec4(r, g, b, 1.0f))
{
}

Color::Color(float r, float g, float b, float a) : values(glm::vec4(r, g, b, a))
{
}

uint32_t Color::get_hex_color() const
{
    // color values > 1.0 are allowed, but they need to be limited when the color gets converted to hex representation
    uint32_t color = glm::clamp(static_cast<int>(values.a * 255.9f), 0, 255);
    color <<= 8;
    color += glm::clamp(static_cast<int>(values.b * 255.999f), 0, 255);
    color <<= 8;
    color += glm::clamp(static_cast<int>(values.g * 255.999f), 0, 255);
    color <<= 8;
    color += glm::clamp(static_cast<int>(values.r * 255.999f), 0, 255);
    return color;
}
