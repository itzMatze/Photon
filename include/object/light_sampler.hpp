#pragma once

#include "object/light.hpp"
#include "renderer/hit_info.hpp"
#include "scene/geometry.hpp"
#include "util/discrete_distribution.hpp"
#include "util/random_generator.hpp"

struct LightSample
{
  glm::vec3 pos;
  glm::vec3 emission;
  double pdf;
};

class LightSampler
{
public:
  LightSampler(const std::vector<Light>& lights, const std::shared_ptr<Geometry> geometry);
  LightSample sample(const HitInfo& hit_info, RandomGenerator& rnd) const;
private:
  DiscreteDistribution<> mesh_lights_distribution;
  const std::vector<Light>& lights;
  // distribution over lights
  DiscreteDistribution<> light_distribution;

  const std::shared_ptr<const Geometry> geometry;
  struct InstanceDistributionsEntry
  {
    DiscreteDistribution<> mesh_distribution;
    uint32_t triangle_distributions_offset;
    uint32_t triangle_distributions_count;
  };
  // store the indices of the instances that contain emissive meshes
  std::vector<uint32_t> emissive_instance_indices;
  // distribution over instances
  DiscreteDistribution<> instance_distribution;
  // distributions over meshes of each instance
  std::vector<InstanceDistributionsEntry> mesh_distributions;
  // distributions over triangles of each mesh of each instance
  std::vector<DiscreteDistribution<>> triangle_distributions;
};
