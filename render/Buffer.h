#ifndef RAY_TRACER_CPP_BUFFER_H
#define RAY_TRACER_CPP_BUFFER_H
#include "defines.h"

class Buffer
{
public:
    Buffer() = default;
    void init(int width, int height);
    Color get_pixel(int i, int j) const;
    void set_pixel(int i, int j, Color color);
    Color* get_texture();
    int get_width() const;
    int get_height() const;
private:
    Color* texture;
    int width, height;
};

#endif //RAY_TRACER_CPP_BUFFER_H
