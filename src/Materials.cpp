#include "Materials.h"
#include <iostream>

bool Dielectric::scatter(const Ray& r_in, const RayPayload& rp, glm::vec4& attenuation, RandomGenerator* random_generator, Ray& scattered) const
{
    glm::vec3 outward_normal;
    attenuation = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    float ref_idx;
#if 1
    ref_idx = sqrt((0.3306f * rp.wavelength * rp.wavelength) / (rp.wavelength * rp.wavelength - 175.0f * 175.0f) + 
        (4.3356f * rp.wavelength * rp.wavelength) / (rp.wavelength * rp.wavelength - 106.0f * 106.0f) + 1);
#else
    ref_idx = sqrt((1.03961212f * rp.wavelength * rp.wavelength) / (rp.wavelength * rp.wavelength - 0.00600069897f) + 
        (0.231792344f * rp.wavelength * rp.wavelength) / (rp.wavelength * rp.wavelength - 0.0200179144f) + 
        (1.01046945f * rp.wavelength * rp.wavelength) / (rp.wavelength * rp.wavelength - 103.560653f) + 1);
#endif
    constexpr float n_1 = 1.0f;
    float reflect_prob, ni_over_nt;
    if (glm::dot(r_in.direction(), rp.normal) > 0)
    {
        outward_normal = -rp.normal;
        ni_over_nt = ref_idx;
        reflect_prob = reflectance(glm::dot(r_in.direction(), rp.normal), ref_idx, n_1);
    }
    else
    {
        outward_normal = rp.normal;
        ni_over_nt = n_1 / ref_idx;
        reflect_prob = reflectance(glm::dot(r_in.direction(), -rp.normal), n_1, ref_idx);
    }
    glm::vec3 refracted = glm::refract(r_in.direction(), outward_normal, ni_over_nt);
    if (!(glm::length(refracted) > 0.0f) || random_generator->random_num() < reflect_prob)
    {
        scattered = Ray(rp.p, glm::reflect(r_in.direction(), outward_normal));
    }
    else
    {
        scattered = Ray(rp.p, refracted);
    }
    return true;
}

float Dielectric::reflectance(float cosine, float n_1, float n_2)
{
    // use Schlick's approximation for reflectance
    float r0 = (n_1 - n_2) / (n_1 + n_2);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}

bool Lambertian::scatter(const Ray& r_in, const RayPayload& rp, glm::vec4& attenuation, RandomGenerator* random_generator, Ray& scattered) const
{
    // TODO probably the scattered ray should be created with a direction in a hemisphere over the current point
    // diffuse material: scatter randomly anywhere, add normal to make sure the ray points away from the surface
    scattered = Ray(rp.p, random_generator->random_cosine_weighted_hemisphere(rp.normal));
    attenuation = albedo->value(rp.uv.x, rp.uv.y, rp.p).values;
    return true;
}

bool Metal::scatter(const Ray& r_in, const RayPayload& rp, glm::vec4& attenuation, RandomGenerator* random_generator, Ray& scattered) const
{
    // reflective material: create reflected ray with a little offset depending on the roughness of the surface
    glm::vec3 reflected = glm::reflect(glm::normalize(r_in.direction()), rp.normal);
    scattered = Ray(rp.p, reflected + fuzz * glm::vec3(random_generator->random_num(), random_generator->random_num(), random_generator->random_num()));
    attenuation = albedo->value(0, 0, rp.p).values;
    return (glm::dot(scattered.direction(), rp.normal) > 0);
}
