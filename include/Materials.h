#pragma once

#include "RandomGenerator.h"
#include "glm/glm.hpp"
#include "RayPayload.h"
#include "Ray.h"
#include "Textures.h"

class Material
{
public:
    virtual bool scatter(const Ray& r_in, const RayPayload& rp, glm::vec4& attenuation, RandomGenerator* random_generator, Ray& scattered) const = 0;
    virtual Color emitted(glm::vec2& uv, const glm::vec3& p) const
    {
        return Color(0.0f, 0.0f, 0.0f);
    }
};

class Dielectric : public Material
{
public:
    explicit Dielectric(float ri) : ref_idx(ri)
    {}

    bool scatter(const Ray& r_in, const RayPayload& rp, glm::vec4& attenuation, RandomGenerator* random_generator, Ray& scattered) const override;

private:
    static float reflectance(float cosine, float ref_idx);

    float ref_idx;
};

class Lambertian : public Material
{
public:
    explicit Lambertian(const std::shared_ptr<Texture> a) : albedo(a)
    {}
    explicit Lambertian(const glm::vec3 a)
    {
        albedo = std::make_shared<ConstantTexture>(Color(a.x, a.y, a.z));
    }

    bool scatter(const Ray& r_in, const RayPayload& rp, glm::vec4& attenuation, RandomGenerator* random_generator, Ray& scattered) const override;

    std::shared_ptr<Texture> albedo;
};

class Metal : public Material
{
public:
    Metal(const std::shared_ptr<Texture> a, float fuzz) : albedo(a)
    {this->fuzz = fuzz < 1 ? fuzz : 1.0f;}

    Metal(const glm::vec3 a, float fuzz)
    {
        albedo = std::make_shared<ConstantTexture>(Color(a.x, a.y, a.z));
        this->fuzz = fuzz < 1 ? fuzz : 1.0f;
    }

    bool scatter(const Ray& r_in, const RayPayload& rp, glm::vec4& attenuation, RandomGenerator* random_generator, Ray& scattered) const override;

    std::shared_ptr<Texture> albedo;
    float fuzz;
};
