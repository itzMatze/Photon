#pragma once

#include "glm/glm.hpp"

#include "sdl_window/Color.h"

#include <memory>

class Texture
{
public:
    virtual Color value(float u, float v, const glm::vec3& p) const = 0;
};

class ConstantTexture : public Texture
{
public:
    ConstantTexture();
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
    CheckerTexture();
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
