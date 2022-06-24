#include "materials/Lambertian.h"

bool Lambertian::scatter(const Ray& r_in, const HitRecord& rec, glm::vec4& attenuation, RandomGenerator* random_generator, Ray& scattered) const
{
    // TODO probably the scattered ray should be created with a direction in a hemisphere over the current point
    // diffuse material: scatter randomly anywhere, add normal to make sure the ray points away from the surface
    glm::vec3 target = rec.p + rec.normal + random_generator->random_in_unit_sphere();
    scattered = Ray(rec.p, target - rec.p);
    attenuation = albedo->value(0, 0, rec.p).values;
    return true;
}
