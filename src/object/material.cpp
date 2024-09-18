#include "object/material.hpp"
#include "renderer/hit_info.hpp"
#include "util/random_generator.hpp"
#include "util/vec3.hpp"
#include "glm/geometric.hpp"
#include <memory>

bool MaterialParameters::is_emissive() const
{
  return (glm::dot(emission, emission) > 0.0f && emission_strength > 0.0f);
}

Material::Material(std::shared_ptr<const std::vector<Texture>> textures, const MaterialParameters& params) : textures(textures), params(params)
{}

Color Material::get_albedo(const HitInfo& hit_info) const
{
  if (params.albedo_texture_id == -1) return params.albedo;
  else return (*textures)[params.albedo_texture_id].get_value(hit_info.bary, hit_info.tex_coords);
}

glm::vec3 Material::get_emission(const HitInfo& hit_info) const
{
  if (params.show_bary) return glm::vec3(hit_info.bary.s, hit_info.bary.t, 1.0f);
  else if (params.show_normal && !params.smooth_shading) return glm::vec3((hit_info.geometric_normal + 1.0f) / 2.0f);
  else if (params.show_normal) return glm::vec3((hit_info.normal + 1.0f) / 2.0f);
  else if (params.show_tex_coords) return glm::vec3(hit_info.tex_coords.s, hit_info.tex_coords.t, 1.0f);
  else if (params.show_albedo) return get_albedo(hit_info).value;
  else return params.emission * params.emission_strength;
}

glm::vec3 Material::eval(const HitInfo& hit_info, const glm::vec3& incident_dir, const glm::vec3& outgoing_dir) const
{
  // for dirac delta lobes every direction has a value of 0.0
  if (is_delta()) return glm::vec3(0.0, 0.0, 0.0);
  const glm::vec3 normal = params.smooth_shading ? hit_info.get_oriented_face_normal() : hit_info.get_oriented_face_geometric_normal();
  const float cos_theta = glm::dot(outgoing_dir, normal);
  return (get_albedo(hit_info).value * std::max(0.0f, cos_theta)) / M_PIf;
}

float fresnel_schlick(float cos_theta, float n_1, float n_2)
{
    float F0 = std::pow((n_1 - n_2) / (n_1 + n_2), 2);
    return F0 + (1.0 - F0) * std::pow(1.0 - cos_theta, 5.0);
}

struct DirectionSample
{
  glm::vec3 dir;
  float pdf;
};

DirectionSample random_cosine_weighted_hemisphere(const glm::vec3& normal, RandomGenerator& rnd)
{
  glm::vec2 random_nums = glm::vec2(rnd.random_float(), rnd.random_float());
  float r = std::sqrtf(random_nums.x);
  float theta = 2.0 * M_PI * random_nums.y;

  glm::vec3 axis = glm::abs(normal);
  if(axis.x < axis.y && axis.x < axis.z) axis = glm::vec3(1.0f, 0.0f, 0.0f);
  else if (axis.y < axis.z) axis = glm::vec3(0.0f, 1.0f, 0.0f);
  else axis = glm::vec3(0.0f, 0.0f, 1.0f);

  glm::vec3 bitangent = glm::cross(normal, axis);
  glm::vec3 tangent = glm::cross(bitangent, normal);

  glm::vec3 dir = glm::normalize(glm::vec3(r * std::sinf(theta), std::sqrtf(1.0 - random_nums.x), r * std::cosf(theta)));

  DirectionSample dir_sample{.dir = glm::normalize(dir.x * bitangent + dir.y * normal + dir.z * tangent), .pdf = dir.y / M_PIf};
  return dir_sample;
}

void Material::get_bsdf_samples(const HitInfo& hit_info, const glm::vec3& incident_dir, std::vector<BSDFSample>& samples, RandomGenerator* rnd) const
{
  glm::vec3 normal = params.smooth_shading ? hit_info.normal : hit_info.geometric_normal;
  if (is_delta() && params.metallic == 1.0f)
  {
    BSDFSample sample(Ray(hit_info.pos + RAY_START_OFFSET * normal, glm::normalize(glm::reflect(incident_dir, normal)), {.backface_culling = false}), 1.0f);
    sample.attenuation = get_albedo(hit_info).value;
    samples.push_back(sample);
  }
  else if (is_delta() && params.transmission == 1.0f)
  {
    glm::vec3 attenuation(1.0f);
    // incident vector and normal do not align -> from air to transmissive material
    float ref_idx_one = 1.0;
    float ref_idx_two = params.ior;
    if (glm::dot(incident_dir, normal) > 0.0)
    {
      // incident vector and normal align -> from transmissive material to air
      normal = -normal;
      ref_idx_one = params.ior;
      ref_idx_two = 1.0;
      const Color albedo = get_albedo(hit_info);
      attenuation = (albedo.value * glm::vec4(std::exp(-hit_info.t * (1.0 - albedo.value.a))));
    }
    float fresnel = fresnel_schlick(glm::dot(-incident_dir, normal), ref_idx_one, ref_idx_two);
    // disable backface culling inside of the transmissive object
    BSDFSample refraction_sample(Ray(hit_info.pos - RAY_START_OFFSET * normal, glm::normalize(glm::refract(incident_dir, normal, ref_idx_one / ref_idx_two)), RayConfig{.backface_culling = false}), 1.0f);
    refraction_sample.attenuation = glm::vec3(1.0 - fresnel) * attenuation;
    BSDFSample reflection_sample(Ray(hit_info.pos + RAY_START_OFFSET * normal, glm::normalize(glm::reflect(incident_dir, normal)), RayConfig{.backface_culling = false}), 1.0f);
    reflection_sample.attenuation = glm::vec3(fresnel) * attenuation;
    // refract returns a zero vector for total internal reflection
    if (glm::dot(refraction_sample.ray.get_dir(), refraction_sample.ray.get_dir()) > 0.1) samples.push_back(refraction_sample);
    else reflection_sample.attenuation = glm::vec3(1.0);
    samples.push_back(reflection_sample);
  }
  else if (rnd && !is_delta())
  {
    DirectionSample dir_sample = random_cosine_weighted_hemisphere(normal, *rnd);
    BSDFSample sample(Ray(hit_info.pos + RAY_START_OFFSET * normal, dir_sample.dir, {.backface_culling = false}), get_albedo(hit_info).value, dir_sample.pdf);
    samples.push_back(sample);
  }
}

bool Material::is_delta() const
{
  return (params.metallic == 1.0f || params.transmission == 1.0f) && (params.roughness == 0.0f);
}

bool Material::is_emissive() const
{
  return params.is_emissive();
}
