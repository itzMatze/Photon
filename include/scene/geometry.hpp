#pragma once
#include <vector>
#include "object/bvh.hpp"
#include "object/material.hpp"
#include "object/object.hpp"
#include "object/object_instance.hpp"
#include "renderer/hit_info.hpp"
#include "renderer/ray.hpp"
#include "util/interpolatable_data.hpp"

class Geometry
{
public:
  Geometry() = default;
  Geometry(const std::shared_ptr<const std::vector<Object>>& objects,
           const InterpolatableData<ObjectInstance>& object_instances,
           const std::shared_ptr<const std::vector<Material>>& materials,
           const std::shared_ptr<const std::vector<Texture>>& textures);
  const std::shared_ptr<const std::vector<Object>> get_objects() const;
  const InterpolatableData<ObjectInstance>& get_interpolatable_object_instances() const;
  const std::shared_ptr<const std::vector<Material>> get_materials() const;
  const Material& get_material(uint32_t id) const;
  const std::shared_ptr<const std::vector<Texture>> get_textures() const;
  const Texture& get_texture(uint32_t id) const;
  bool intersect(const Ray& ray, HitInfo& hit_info) const;

private:
  std::shared_ptr<const std::vector<Object>> objects;
  InterpolatableData<ObjectInstance> instances;
  std::shared_ptr<const std::vector<Material>> materials;
  std::shared_ptr<const std::vector<Texture>> textures;
  BVH<ObjectInstance> bvh;
};

Geometry interpolate(const Geometry& a, const Geometry& b, float weight);