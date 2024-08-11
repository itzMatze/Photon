#pragma once

#include <memory>
#include <vector>
#include "object/texture.hpp"
#include "renderer/hit_info.hpp"
#include "renderer/ray.hpp"
#include "util/vec3.hpp"

enum class MaterialType
{
  Refractive,
  Reflective,
  Diffuse
};

struct MaterialParameters
{
  MaterialType type;
  glm::vec3 albedo = glm::vec3(0.0f);
  int32_t albedo_texture_id = -1;
  float ior = 1.5f;
  float roughness = 0.0f;
  bool smooth_shading = true;
  bool show_albedo = false;
  bool show_bary = false;
  bool show_normal = false;
  bool show_tex_coords = false;
};

struct BSDFSample
{
  BSDFSample(const Ray& ray) : ray(ray), attenuation(1.0, 1.0, 1.0)
  {}
  BSDFSample(const Ray& ray, const glm::vec3& attenuation) : ray(ray), attenuation(attenuation)
  {}
  Ray ray;
  glm::vec3 attenuation;
};

class Material
{
public:
  Material(std::shared_ptr<const std::vector<Texture>> textures, const MaterialParameters& params);
  glm::vec3 get_albedo(const HitInfo& hit_info) const;
  glm::vec3 eval(const HitInfo& hit_info, const glm::vec3& incident_dir, const glm::vec3& outgoing_dir) const;
  void get_bsdf_samples(const HitInfo& hit_info, const glm::vec3& incident_dir, std::vector<BSDFSample>& samples) const;
  // is material dirac delta reflective or refractive
  bool is_delta() const;
  bool is_light_dependent() const;

private:
  std::shared_ptr<const std::vector<Texture>> textures;
  MaterialParameters params;
};
