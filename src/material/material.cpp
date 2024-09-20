#include "material/material.hpp"
#include "renderer/hit_info.hpp"
#include "util/random_generator.hpp"
#include "util/vec3.hpp"
#include "glm/geometric.hpp"

bool MaterialParameters::is_emissive() const
{
  return (glm::dot(emission, emission) > 0.0f && emission_strength > 0.0f);
}

Material::Material() : params{.albedo_texture_id = -1}, textures(nullptr) {};

Material::Material(const MaterialParameters& params, std::shared_ptr<const std::vector<Texture>> textures) : params(params), textures(textures) {};

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

float Material::fresnel_schlick(float cos_theta, float n_1, float n_2)
{
  float F0 = std::pow((n_1 - n_2) / (n_1 + n_2), 2);
  return F0 + (1.0 - F0) * std::pow(1.0 - cos_theta, 5.0);
}

void Material::get_bsdf_samples(const HitInfo& hit_info, const glm::vec3& incident_dir, std::vector<BSDFSample>& samples, RandomGenerator* rnd) const
{
  if (is_delta() && params.metallic == 1.0f)
  {
    metal_add_bsdf_samples(hit_info, incident_dir, samples, rnd);
  }
  else if (is_delta() && params.transmission == 1.0f)
  {
    dielectric_add_bsdf_samples(hit_info, incident_dir, samples, rnd);
  }
  else if (rnd && !is_delta())
  {
    diffuse_add_bsdf_samples(hit_info, incident_dir, samples, rnd);
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
