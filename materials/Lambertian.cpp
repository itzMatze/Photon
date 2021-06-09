#include "Lambertian.h"

bool Lambertian::scatter(const Ray& r_in, const HitRecord& rec, glm::vec4& attenuation, Ray& scattered) const
{
    // TODO probably the scattered ray should be created with a direction in a hemisphere over the current point
    // diffuse material: scatter randomly anywhere, add normal to make sure the ray points away from the surface
    glm::vec3 target = rec.p + rec.normal + random_in_unit_sphere();
    scattered = Ray(rec.p, target - rec.p);
    attenuation = glm::vec4(albedo, 1.0f);
    return true;
}
