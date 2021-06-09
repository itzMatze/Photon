#include "Metal.h"

bool Metal::scatter(const Ray& r_in, const HitRecord& rec, glm::vec4& attenuation, Ray& scattered) const
{
    // reflective material: create reflected ray with a little offset depending on the roughness of the surface
    glm::vec3 reflected = glm::reflect(glm::normalize(r_in.direction), rec.normal);
    scattered = Ray(rec.p, reflected + fuzz * random_in_unit_sphere());
    attenuation = glm::vec4(albedo, 1.0f);
    return (glm::dot(scattered.direction, rec.normal) > 0);
}
