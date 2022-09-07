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

glm::vec4 Color::wavelength_to_rgba(uint32_t wavelength)
{
    assert(wavelength <= 780);
    assert(wavelength >= 380);
    // divide through average response
    return glm::vec4(xyz_to_rgb(cie_colour_match[(wavelength - 380) / 5]) * glm::vec3(1.0f / 0.542192f, 1.0f / 0.275452f, 1.0f / 0.347872f), 1.0f);
}

glm::vec3 Color::xyz_to_rgb(const float xyz[3])
{
    // https://www.fourmilab.ch/documents/specrend/
    constexpr float xr = 0.7355f, yr = 0.2645f, zr = 1 - xr - yr, xg = 0.2658f, yg = 0.7243f, zg = 1 - xg - yg, xb = 0.1669f, yb = 0.0085f, zb = 1 - xb - yb;
    constexpr float xw = 0.33333333f, yw = 0.33333333f, zw = 0.33333333f;

    // xyz -> rgb matrix, before scaling to white
    constexpr float rx = (yg * zb) - (yb * zg);
    constexpr float ry = (xb * zg) - (xg * zb);
    constexpr float rz = (xg * yb) - (xb * yg);
    constexpr float gx = (yb * zr) - (yr * zb);
    constexpr float gy = (xr * zb) - (xb * zr);
    constexpr float gz = (xb * yr) - (xr * yb);
    constexpr float bx = (yr * zg) - (yg * zr);
    constexpr float by = (xg * zr) - (xr * zg);
    constexpr float bz = (xr * yg) - (xg * yr);

    // White scaling factors. Dividing by yw scales the white luminance to unity, as conventional
    constexpr float rw = ((rx * xw) + (ry * yw) + (rz * zw)) / yw;
    constexpr float gw = ((gx * xw) + (gy * yw) + (gz * zw)) / yw;
    constexpr float bw = ((bx * xw) + (by * yw) + (bz * zw)) / yw;

    // xyz -> rgb matrix, correctly scaled to white
    constexpr float scaled_rx = rx / rw;
    constexpr float scaled_ry = ry / rw;
    constexpr float scaled_rz = rz / rw;
    constexpr float scaled_gx = gx / gw;
    constexpr float scaled_gy = gy / gw;
    constexpr float scaled_gz = gz / gw;
    constexpr float scaled_bx = bx / bw;
    constexpr float scaled_by = by / bw;
    constexpr float scaled_bz = bz / bw;

    // rgb of the desired point
    glm::vec3 rgb;
    rgb.r = (scaled_rx * xyz[0]) + (scaled_ry * xyz[1]) + (scaled_rz * xyz[2]);
    rgb.g = (scaled_gx * xyz[0]) + (scaled_gy * xyz[1]) + (scaled_gz * xyz[2]);
    rgb.b = (scaled_bx * xyz[0]) + (scaled_by * xyz[1]) + (scaled_bz * xyz[2]);

    // Amount of white needed is w = - min(0, r, g, b)
    float w = (0.0f < rgb.r) ? 0.0f : rgb.r;
    w = (w < rgb.g) ? w : rgb.g;
    w = (w < rgb.b) ? w : rgb.b;
    w = -w;

    // Add just enough white to make r, g, b all positive
    rgb.r += w;
    rgb.g += w;
    rgb.b += w;

    float greatest = std::max(rgb.r, std::max(rgb.g, rgb.b));
    if (greatest > 0.0f) {
        rgb.r /= greatest;
        rgb.g /= greatest;
        rgb.b /= greatest;
    }

    return rgb;
}
