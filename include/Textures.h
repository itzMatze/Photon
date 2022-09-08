#pragma once

#include "glm/glm.hpp"

#include "sdl_window/Color.h"
#include "Perlin.h"

#include <memory>

class Texture
{
public:
    virtual Color value(float u, float v, const glm::vec3& p) const = 0;
};

class ConstantTexture : public Texture
{
public:
    ConstantTexture() = default;
    ConstantTexture(Color c) : color(c)
    {}
    virtual Color value(float u, float v, const glm::vec3& p) const override
    {
        return color;
    }

private:
    Color color;
};

class CheckerTexture : public Texture
{
public:
    CheckerTexture() = default;
    CheckerTexture(const std::shared_ptr<Texture> t0, const std::shared_ptr<Texture> t1) : even(t0), odd(t1)
    {}
    virtual Color value(float u, float v, const glm::vec3& p) const override
    {
        float sines = glm::sin(10 * p.x) * glm::sin(10 * p.z);
        if (sines < 0)
        {
            return odd->value(u, v, p);
        }
        else
        {
            return even->value(u, v, p);
        }
    }
    
private:
    std::shared_ptr<Texture> even;
    std::shared_ptr<Texture> odd;
};

class NoiseTexture : public Texture
{
public:
    NoiseTexture(const float freq, const bool smooth) : freq(freq), smooth(smooth)
    {}
    virtual Color value(float u, float v, const glm::vec3& p) const override
    {
        float rnd = smooth ? 0.5f * (1.0f + noise.smooth_noise(freq * p)) : noise.noise(freq * p);
        //rnd = 0.5f * (1.0f + noise.turb(freq * p));
        //rnd = noise.turb(freq * p);
        rnd = 0.5f * (1.0f + glm::sin(freq * p.z + 30.0f * noise.turb(p)));
        return Color(rnd, rnd, rnd);
    }

private:
Perlin noise;
const float freq;
const bool smooth;
};

class ImageTexture : public Texture
{
public:
    ImageTexture() = default;
    ImageTexture(unsigned char* pixels, int x, int y, int n) : data(pixels), nx(x), ny(y), nc(n)
    {}
    virtual Color value(float u, float v, const glm::vec3& p) const override
    {
        int i = u * nx;
        int j = (1 - v) * ny;
        i = glm::clamp(i, 0, nx - 1);
        j = glm::clamp(j, 0, ny - 1);
        float r = static_cast<int>(data[nc*i + nc*nx*j]) / 255.0f;
        float g = static_cast<int>(data[nc*i + nc*nx*j + 1]) / 255.0f;
        float b = static_cast<int>(data[nc*i + nc*nx*j + 2]) / 255.0f;
        return Color(r, g, b);
    }

private:
    unsigned char* data;
    int nx, ny, nc;
};
