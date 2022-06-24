#pragma once

#include "Material.h"

class Metal : public Material
{
public:
    Metal(const std::shared_ptr<Texture> a, float fuzz) : albedo(a)
    {this->fuzz = fuzz < 1 ? fuzz : 1.0f;}

    Metal(const glm::vec3 a, float fuzz)
    {
        albedo = std::make_shared<ConstantTexture>(Color(a.x, a.y, a.z));
        this->fuzz = fuzz < 1 ? fuzz : 1.0f;
    }

    bool scatter(const Ray& r_in, const HitRecord& rec, glm::vec4& attenuation, RandomGenerator* random_generator, Ray& scattered) const override;

    std::shared_ptr<Texture> albedo;
    float fuzz;
};
