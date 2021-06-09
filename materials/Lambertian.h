#ifndef RAY_TRACING_IOW_LAMBERTIAN_H
#define RAY_TRACING_IOW_LAMBERTIAN_H

#include "Material.h"

class Lambertian : public Material
{
public:
    explicit Lambertian(const glm::vec3 a) : albedo(a)
    {}

    bool scatter(const Ray& r_in, const HitRecord& rec, glm::vec4& attenuation, Ray& scattered) const override;

    glm::vec3 albedo;
};

#endif //RAY_TRACING_IOW_LAMBERTIAN_H
