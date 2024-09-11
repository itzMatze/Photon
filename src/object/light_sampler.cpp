#include "object/light_sampler.hpp"
#include "glm/geometric.hpp"
#include "object/object_instance.hpp"
#include "util/random_generator.hpp"
#include "util/vec3.hpp"

LightSampler::LightSampler(const std::vector<Light>& lights, const std::shared_ptr<Geometry> geometry) : lights(lights), geometry(geometry)
{
  const std::vector<ObjectInstance>& object_instances = geometry->get_interpolatable_object_instances().get_data();
  // create uniform distribution for everything
  if (lights.size() > 0)
  {
    std::vector<uint32_t> light_weights(lights.size(), 1);
    light_distribution = DiscreteDistribution(light_weights);
  }
  for (uint32_t i = 0; i < object_instances.size(); i++)
  {
    const ObjectInstance& instance = object_instances[i];
    // an instance can be assigned a material that overrides the mesh materials
    if ((instance.material_id == -1 && instance.get_object().get_emissive_meshes().size() > 0) || instance.is_emissive) emissive_instance_indices.push_back(i);
  }
  if (emissive_instance_indices.size() > 0)
  {
    std::vector<uint32_t> instance_weights(emissive_instance_indices.size(), 1);
    instance_distribution = DiscreteDistribution(instance_weights);
    std::vector<uint32_t> mesh_weights;
    std::vector<uint32_t> triangle_weights;
    // create a separate distribution over meshes for each instance
    for (uint32_t i = 0; i < emissive_instance_indices.size(); i++)
    {
      const ObjectInstance& instance = object_instances[emissive_instance_indices[i]];
      const std::vector<Mesh>& emissive_meshes = instance.get_object().get_emissive_meshes();
      // if instance does not contain emissive meshes it is emissive by itself
      const uint32_t emissive_mesh_count = std::max(1ul, emissive_meshes.size());
      mesh_weights.resize(emissive_mesh_count, 1);
      InstanceDistributionsEntry instance_distributions_entry;
      instance_distributions_entry.mesh_distribution = DiscreteDistribution(mesh_weights);
      instance_distributions_entry.triangle_distributions_offset = triangle_distributions.size();
      instance_distributions_entry.triangle_distributions_count = emissive_mesh_count;
      mesh_distributions.push_back(instance_distributions_entry);
      if (emissive_meshes.size() > 0)
      {
        // create a separate distribution over triangles for each mesh in an instance
        for (const Mesh& mesh : emissive_meshes)
        {
          triangle_weights.resize(mesh.triangle_index_count, 1);
          triangle_distributions.push_back(DiscreteDistribution(triangle_weights));
        }
      }
      else
    {
        triangle_weights.resize(instance.get_object().get_triangles().size(), 1);
        triangle_distributions.push_back(DiscreteDistribution(triangle_weights));
      }
    }
  }
  std::vector<uint32_t> mesh_lights_weights({uint32_t(emissive_instance_indices.size() * 2), uint32_t(lights.size())});
  mesh_lights_distribution = DiscreteDistribution(mesh_lights_weights);
}

LightSample LightSampler::sample(const HitInfo& hit_info, RandomGenerator& rnd) const
{
  const std::vector<ObjectInstance>& object_instances = geometry->get_interpolatable_object_instances().get_data();
  uint32_t mesh_lights_index = mesh_lights_distribution.sample(rnd);
  LightSample light_sample{.pdf = mesh_lights_distribution.get_probability(mesh_lights_index)};
  if (mesh_lights_index == 0)
  {
    // sample instance and get probability of the chosen sample
    uint32_t instance_index = instance_distribution.sample(rnd);
    light_sample.pdf *= instance_distribution.get_probability(instance_index);
    const ObjectInstance& instance = object_instances[emissive_instance_indices[instance_index]];
    const InstanceDistributionsEntry& instance_distributions_entry = mesh_distributions[instance_index];
    int32_t material_id = instance.material_id;
    uint32_t triangle_index;
    if (material_id == -1)
    {
      // sample mesh and get probability of the chosen sample
      uint32_t mesh_index = instance_distributions_entry.mesh_distribution.sample(rnd);
      light_sample.pdf *= instance_distributions_entry.mesh_distribution.get_probability(mesh_index);
      const Mesh& mesh = instance.get_object().get_emissive_meshes()[mesh_index];
      // sample triangle and get probability of the chosen sample
      triangle_index = triangle_distributions[instance_distributions_entry.triangle_distributions_offset + mesh_index].sample(rnd);
      light_sample.pdf *= triangle_distributions[instance_distributions_entry.triangle_distributions_offset + mesh_index].get_probability(triangle_index);
      triangle_index += mesh.triangle_index_offset;
      material_id = mesh.material_id;
    }
    else
    {
      // if instance is emissive directly sample a triangle from the instance
      triangle_index = triangle_distributions[instance_distributions_entry.triangle_distributions_offset].sample(rnd);
      light_sample.pdf *= triangle_distributions[instance_distributions_entry.triangle_distributions_offset].get_probability(triangle_index);
    }
    const Triangle& triangle = instance.get_object().get_triangles()[triangle_index];
    // sample random point on triangle and compute probability of the sampled point
    // triangle vertices need to be converted to world space for the correct triangle area
    glm::vec3 pos0 = instance.get_spatial_conf().transform_pos(triangle.get_triangle_vertex(0).pos);
    glm::vec3 pos1 = instance.get_spatial_conf().transform_pos(triangle.get_triangle_vertex(1).pos);
    glm::vec3 pos2 = instance.get_spatial_conf().transform_pos(triangle.get_triangle_vertex(2).pos);
    glm::vec2 bary = rnd.random_barycentrics();
    light_sample.pos = bary.s * pos0 + bary.t * pos1 + (1.0f - bary.s - bary.t) * pos2;
    double triangle_area = glm::length(glm::cross(pos1 - pos0, pos2 - pos0)) / 2.0;
    light_sample.pdf *= 1.0 / triangle_area;
    const float distance = glm::distance(hit_info.pos, light_sample.pos);
    const float cos_theta = std::max(0.0f, glm::dot(glm::normalize(instance.get_spatial_conf().transform_dir(triangle.get_geometric_normal())), glm::normalize(hit_info.pos - light_sample.pos)));
    light_sample.emission = (geometry->get_material(material_id).get_emission(hit_info) * cos_theta) / glm::vec3(light_sample.pdf * distance * distance);
  }
  else
  {
    uint32_t light_index = light_distribution.sample(rnd);
    light_sample.pdf *= light_distribution.get_probability(light_index);
    light_sample.pos = lights[light_index].get_position();
    light_sample.emission = lights[light_index].get_contribution(hit_info.pos) / glm::vec3(light_sample.pdf);
  }
  return light_sample;
}
