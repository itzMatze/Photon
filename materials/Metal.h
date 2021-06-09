#ifndef RAY_TRACING_IOW_METAL_H
#define RAY_TRACING_IOW_METAL_H

#include "Material.h"

class Metal : public Material
{
public:
    Metal(const glm::vec3 a, float fuzz) : albedo(a)
    {this->fuzz = fuzz < 1 ? fuzz : 1.0f;}

    bool scatter(const Ray& r_in, const HitRecord& rec, glm::vec4& attenuation, Ray& scattered) const override;

    glm::vec3 albedo;
    float fuzz;
};

#endif //RAY_TRACING_IOW_METAL_H
