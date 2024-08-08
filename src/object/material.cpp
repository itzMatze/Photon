#include "object/material.hpp"
#include "renderer/hit_info.hpp"
#include "util/vec.hpp"

Material::Material() : type(MaterialType::Diffuse)
{}

Material::Material(MaterialType type, const MaterialParameters& params) : type(type), params(params)
{}

cm::Vec3 Material::get_albedo(const HitInfo& hit_info) const
{
  if (params.show_bary) return cm::Vec3(hit_info.bary.u, hit_info.bary.v, 1.0);
  else if (params.show_normal && !params.smooth_shading) return cm::Vec3((hit_info.geometric_normal + 1.0) / 2.0);
  else if (params.show_normal) return cm::Vec3((hit_info.normal + 1.0) / 2.0);
  else if (params.show_tex_coords) return cm::Vec3(hit_info.tex_coords.u, hit_info.tex_coords.v, 1.0);
  else return params.albedo_texture->get_value(hit_info.bary, hit_info.tex_coords);
}

cm::Vec3 Material::eval(const HitInfo& hit_info, const cm::Vec3& incident_dir, const cm::Vec3& outgoing_dir) const
{
  // for dirac delta lobes every direction has a value of 0.0
  if (is_delta()) return cm::Vec3(0.0, 0.0, 0.0);
  const float cos_theta = cm::dot(outgoing_dir, params.smooth_shading ? hit_info.normal : hit_info.geometric_normal);
  return get_albedo(hit_info) * std::max(0.0f, cos_theta);
}

float fresnel_schlick(float cos_theta, float n_1, float n_2)
{
    float F0 = std::pow((n_1 - n_2) / (n_1 + n_2), 2);
    return F0 + (1.0 - F0) * std::pow(1.0 - cos_theta, 5.0);
}

std::vector<BSDFSample> Material::get_bsdf_samples(const HitInfo& hit_info, const cm::Vec3& incident_dir) const
{
  cm::Vec3 normal = params.smooth_shading ? hit_info.normal : hit_info.geometric_normal;
  // return list of direction that need to be traced next
  std::vector<BSDFSample> samples;
  if (is_delta() && type == MaterialType::Reflective)
  {
    BSDFSample sample(Ray(hit_info.pos + RAY_START_OFFSET * normal, cm::normalize(cm::reflect(incident_dir, normal))));
    sample.attenuation = params.albedo_texture->get_value(hit_info.bary, hit_info.tex_coords);
    samples.push_back(sample);
  }
  else if (is_delta() && type == MaterialType::Refractive)
  {
    // incident vector and normal do not align -> from air to transmissive material
    float ref_idx_one = 1.0;
    float ref_idx_two = params.ior;
    if (cm::dot(incident_dir, normal) > 0.0)
    {
      // incident vector and normal align -> from transmissive material to air
      normal = -normal;
      ref_idx_one = params.ior;
      ref_idx_two = 1.0;
    }
    float fresnel = fresnel_schlick(cm::dot(-incident_dir, normal), ref_idx_one, ref_idx_two);
    // disable backface culling inside of the transmissive object
    BSDFSample refraction_sample(Ray(hit_info.pos - RAY_START_OFFSET * normal, cm::normalize(cm::refract(incident_dir, normal, ref_idx_one / ref_idx_two)), RayConfig{.backface_culling = false}));
    refraction_sample.attenuation = cm::Vec3(1.0 - fresnel);
    BSDFSample reflection_sample(Ray(hit_info.pos + RAY_START_OFFSET * normal, cm::normalize(cm::reflect(incident_dir, normal)), RayConfig{.backface_culling = false}));
    reflection_sample.attenuation = cm::Vec3(fresnel);
    // refract returns a zero vector for total internal reflection
    if (cm::dot(refraction_sample.ray.get_dir(), refraction_sample.ray.get_dir()) > 0.1) samples.push_back(refraction_sample);
    else reflection_sample.attenuation = cm::Vec3(1.0);
    samples.push_back(reflection_sample);
  }
  return samples;
}

bool Material::is_delta() const
{
  return (type == MaterialType::Reflective || type == MaterialType::Refractive);
}

bool Material::is_light_dependent() const
{
  return (!params.show_albedo && !params.show_bary && !params.show_normal && !params.show_tex_coords);
}
