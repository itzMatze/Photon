#pragma once

#include "Material.h"

class Dielectric : public Material
{
public:
    explicit Dielectric(float ri) : ref_idx(ri)
    {}

    bool scatter(const Ray& r_in, const HitRecord& rec, glm::vec4& attenuation, RandomGenerator* random_generator, Ray& scattered) const override;

private:
    static float reflectance(float cosine, float ref_idx);

    float ref_idx;
};
