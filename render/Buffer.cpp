#include "Buffer.h"

void Buffer::init(int w, int h)
{
    width = w;
    height = h;
    texture = new Color[width * height];
    for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < height; ++j)
        {
            set_pixel(i, j, Color{0, 0, 255, 255});
        }
    }
}

Color Buffer::get_pixel(int i, int j) const
{
    return texture[i + width * j];
}

void Buffer::set_pixel(int i, int j, Color color)
{
    texture[i + width * j] = color;
}

Color *Buffer::get_texture()
{
    return texture;
}

int Buffer::get_width() const
{
    return width;
}

int Buffer::get_height() const
{
    return height;
}

#if 0
for (int i = 0; i < width; ++i)
{
    for (int j = 0; j < height; ++j)
    {
        unsigned int bytePerPixel = 4;
        unsigned char* my_offset = my_buffer + (i + width * j) * bytePerPixel;
        my_offset[0] = i % 255;
        my_offset[1] = 0;
        my_offset[2] = j % 255;
        my_offset[3] = 255;
    }
}
#endif