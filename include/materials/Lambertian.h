#pragma once

#include "Material.h"

class Lambertian : public Material
{
public:
    explicit Lambertian(const glm::vec3 a) : albedo(a)
    {}

    bool scatter(const Ray& r_in, const HitRecord& rec, glm::vec4& attenuation, RandomGenerator* random_generator, Ray& scattered) const override;

    glm::vec3 albedo;
};
