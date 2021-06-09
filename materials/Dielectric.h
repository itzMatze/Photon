#ifndef RAY_TRACING_IOW_DIELECTRIC_H
#define RAY_TRACING_IOW_DIELECTRIC_H

#include "Material.h"

float random_num();

class Dielectric : public Material
{
public:
    explicit Dielectric(float ri) : ref_idx(ri)
    {}

    bool scatter(const Ray& r_in, const HitRecord& rec, glm::vec4& attenuation, Ray& scattered) const override;

private:
    static float reflectance(float cosine, float ref_idx);

    float ref_idx;
};

#endif //RAY_TRACING_IOW_DIELECTRIC_H
