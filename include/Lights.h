#pragma once

#include "Materials.h"

class DiffuseLight : public Material
{
public:
    explicit DiffuseLight(std::shared_ptr<Texture> a) : emit(a)
    {}
    explicit DiffuseLight(const glm::vec3 a)
    {
        emit = std::make_shared<ConstantTexture>(Color(a.x, a.y, a.z));
    }

    bool scatter(const Ray& r_in, const HitRecord& rec, glm::vec4& attenuation, RandomGenerator* random_generator, Ray& scattered) const override
    {
        return false;
    }

    Color emitted(glm::vec2& uv, const glm::vec3& p) const override
    {
        return emit->value(uv.x, uv.y, p);
    }
private:
    std::shared_ptr<Texture> emit;
};
