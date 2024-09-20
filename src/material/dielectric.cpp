#include "material/material.hpp"
#include "glm/geometric.hpp"



void Material::dielectric_add_bsdf_samples(const HitInfo& hit_info, const glm::vec3& incident_dir, std::vector<BSDFSample>& samples, RandomGenerator* rnd) const
{
  glm::vec3 normal = params.smooth_shading ? hit_info.normal : hit_info.geometric_normal;
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
