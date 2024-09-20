#include "material/material.hpp"
#include "glm/geometric.hpp"

void Material::metal_add_bsdf_samples(const HitInfo& hit_info, const glm::vec3& incident_dir, std::vector<BSDFSample>& samples, RandomGenerator* rnd) const
{
  glm::vec3 normal = params.smooth_shading ? hit_info.normal : hit_info.geometric_normal;
  BSDFSample sample(Ray(hit_info.pos + RAY_START_OFFSET * normal, glm::normalize(glm::reflect(incident_dir, normal)), {.backface_culling = false}), 1.0f);
  sample.attenuation = get_albedo(hit_info).value;
  samples.push_back(sample);
}
