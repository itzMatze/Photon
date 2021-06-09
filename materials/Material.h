#ifndef RAY_TRACING_IOW_MATERIAL_H
#define RAY_TRACING_IOW_MATERIAL_H

#include "glm.hpp"
#include "HitRecord.h"
#include "Ray.h"

glm::vec3 random_in_unit_sphere();

class Material
{
public:
    virtual bool scatter(const Ray& r_in, const HitRecord& rec, glm::vec4& attenuation, Ray& scattered) const = 0;
};

#endif //RAY_TRACING_IOW_MATERIAL_H
