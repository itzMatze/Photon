#pragma once

#include <memory>
#include <vector>
#include "material/texture.hpp"
#include "renderer/hit_info.hpp"
#include "renderer/ray.hpp"
#include "util/random_generator.hpp"
#include "util/vec3.hpp"

struct BSDFSample
{
  BSDFSample(const Ray& ray, float pdf) : ray(ray), attenuation(1.0), pdf(pdf)
  {}
  BSDFSample(const Ray& ray, const glm::vec3& attenuation, float pdf) : ray(ray), attenuation(attenuation), pdf(pdf)
  {}
  Ray ray;
  glm::vec3 attenuation;
  float pdf;
};

struct MaterialParameters
{
  Color albedo = Color();
  int32_t albedo_texture_id = -1;
  glm::vec3 emission = glm::vec3(0.0f);
  float emission_strength = 1.0f;
  float roughness = 0.0f;
  float metallic = 0.0f;
  float transmission = 0.0f;
  float ior = 1.5f;
  bool smooth_shading = true;
  bool show_albedo = false;
  bool show_bary = false;
  bool show_normal = false;
  bool show_tex_coords = false;

  bool is_emissive() const;
};

class Material
{
public:
  Material();
  Material(const MaterialParameters& params, std::shared_ptr<const std::vector<Texture>> textures);

  Color get_albedo(const HitInfo& hit_info) const;
  glm::vec3 get_emission(const HitInfo& hit_info) const;
  glm::vec3 eval(const HitInfo& hit_info, const glm::vec3& incident_dir, const glm::vec3& outgoing_dir) const;
  void get_bsdf_samples(const HitInfo& hit_info, const glm::vec3& incident_dir, std::vector<BSDFSample>& samples, RandomGenerator* rnd = nullptr) const;
  // is material dirac delta reflective or refractive
  bool is_delta() const;
  bool is_emissive() const;

private:
  const MaterialParameters params;
  const std::shared_ptr<const std::vector<Texture>> textures;

  struct DirectionSample
  {
    glm::vec3 dir;
    float pdf;
  };
  static float fresnel_schlick(float cos_theta, float n_1, float n_2);
  static DirectionSample random_cosine_weighted_hemisphere(const glm::vec3& normal, RandomGenerator& rnd);
  void diffuse_add_bsdf_samples(const HitInfo& hit_info, const glm::vec3& incident_dir, std::vector<BSDFSample>& samples, RandomGenerator* rnd) const;
  void dielectric_add_bsdf_samples(const HitInfo& hit_info, const glm::vec3& incident_dir, std::vector<BSDFSample>& samples, RandomGenerator* rnd) const;
  void metal_add_bsdf_samples(const HitInfo& hit_info, const glm::vec3& incident_dir, std::vector<BSDFSample>& samples, RandomGenerator* rnd) const;
};
