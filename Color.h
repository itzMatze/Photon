//
// Created by matth on 01.05.2021.
//

#ifndef RAY_TRACER_CPP_COLOR_H
#define RAY_TRACER_CPP_COLOR_H

class Color
{
public:
    Color();
    Color(float r, float g, float b);
    Color(float r, float g, float b, float a);
    unsigned int get_hex_color() const;
private:
    float r, g, b, a;
};

#endif //RAY_TRACER_CPP_COLOR_H
