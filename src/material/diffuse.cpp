#include "material/material.hpp"
#include "glm/geometric.hpp"

Material::DirectionSample Material::random_cosine_weighted_hemisphere(const glm::vec3& normal, RandomGenerator& rnd)
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

void Material::diffuse_add_bsdf_samples(const HitInfo& hit_info, const glm::vec3& incident_dir, std::vector<BSDFSample>& samples, RandomGenerator* rnd) const
{
  glm::vec3 normal = params.smooth_shading ? hit_info.normal : hit_info.geometric_normal;
  DirectionSample dir_sample = random_cosine_weighted_hemisphere(normal, *rnd);
  BSDFSample sample(Ray(hit_info.pos + RAY_START_OFFSET * normal, dir_sample.dir, {.backface_culling = false}), get_albedo(hit_info).value, dir_sample.pdf);
  samples.push_back(sample);
}
