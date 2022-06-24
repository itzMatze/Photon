#pragma once

#include "Material.h"

class Lambertian : public Material
{
public:
    explicit Lambertian(const std::shared_ptr<Texture> a) : albedo(a)
    {}
    explicit Lambertian(const glm::vec3 a)
    {
        albedo = std::make_shared<ConstantTexture>(Color(a.x, a.y, a.z));
    }

    bool scatter(const Ray& r_in, const HitRecord& rec, glm::vec4& attenuation, RandomGenerator* random_generator, Ray& scattered) const override;

    std::shared_ptr<Texture> albedo;
};
