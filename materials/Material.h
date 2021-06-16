#ifndef RAY_TRACING_IOW_MATERIAL_H
#define RAY_TRACING_IOW_MATERIAL_H

#include "RandomGenerator.h"
#include "glm.hpp"
#include "HitRecord.h"
#include "Ray.h"

class Material
{
public:
    virtual bool scatter(const Ray& r_in, const HitRecord& rec, glm::vec4& attenuation, RandomGenerator* random_generator, Ray& scattered) const = 0;
};

#endif //RAY_TRACING_IOW_MATERIAL_H
