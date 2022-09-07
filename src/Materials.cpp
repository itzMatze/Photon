#include "Materials.h"

bool Dielectric::scatter(const Ray& r_in, const HitRecord& rec, glm::vec4& attenuation, RandomGenerator* random_generator, Ray& scattered) const
{
    glm::vec3 outward_normal;
    glm::vec3 reflected = glm::reflect(r_in.direction(), rec.normal);
    float ni_over_nt;
    attenuation = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    float cosine;
    if (glm::dot(r_in.direction(), rec.normal) > 0)
    {
        outward_normal = -rec.normal;
        ni_over_nt = ref_idx;
        cosine = ref_idx * glm::dot(r_in.direction(), rec.normal) / glm::length(r_in.direction());
    }
    else
    {
        outward_normal = rec.normal;
        ni_over_nt = 1.0f / ref_idx;
        cosine = -glm::dot(r_in.direction(), rec.normal) / glm::length(r_in.direction());
    }
    glm::vec3 refracted = glm::refract(r_in.direction(), outward_normal, ni_over_nt);
    float reflect_prob;
    if (glm::length(refracted) > 0.0f)
    {
        reflect_prob = reflectance(cosine, ref_idx);
    }
    else
    {
        reflect_prob = 1.0f;
    }
    if (random_generator->random_num() < reflect_prob)
    {
        scattered = Ray(rec.p, reflected);
    }
    else
    {
        scattered = Ray(rec.p, refracted);
    }
    return true;
}

float Dielectric::reflectance(float cosine, float ref_idx)
{
    // use Schlick's approximation for reflectance
    float r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}

bool Lambertian::scatter(const Ray& r_in, const HitRecord& rec, glm::vec4& attenuation, RandomGenerator* random_generator, Ray& scattered) const
{
    // TODO probably the scattered ray should be created with a direction in a hemisphere over the current point
    // diffuse material: scatter randomly anywhere, add normal to make sure the ray points away from the surface
    scattered = Ray(rec.p, random_generator->random_cosine_weighted_hemisphere(rec.normal));
    attenuation = albedo->value(rec.uv.x, rec.uv.y, rec.p).values;
    return true;
}

bool Metal::scatter(const Ray& r_in, const HitRecord& rec, glm::vec4& attenuation, RandomGenerator* random_generator, Ray& scattered) const
{
    // reflective material: create reflected ray with a little offset depending on the roughness of the surface
    glm::vec3 reflected = glm::reflect(glm::normalize(r_in.direction()), rec.normal);
    scattered = Ray(rec.p, reflected + fuzz * glm::vec3(random_generator->random_num(), random_generator->random_num(), random_generator->random_num()));
    attenuation = albedo->value(0, 0, rec.p).values;
    return (glm::dot(scattered.direction(), rec.normal) > 0);
}
