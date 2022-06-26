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
    virtual Color value(float u, float v, const glm::vec3& p) const
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
    virtual Color value(float u, float v, const glm::vec3& p) const
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
    virtual Color value(float u, float v, const glm::vec3& p) const
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
