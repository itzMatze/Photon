#pragma once

#include <cstdint>
#include "material/material.hpp"
#include "material/texture.hpp"
#include "object/object.hpp"
#include "scene/geometry.hpp"
#include "util/interpolatable_data.hpp"
#include "util/spatial_configuration.hpp"

class GeometryBuilder
{
public:
  GeometryBuilder();
  uint32_t add_object(const Object& object);
  uint32_t add_object_instance(uint32_t object_id, int32_t material_id = -1, const SpatialConfiguration& spatial_conf = SpatialConfiguration());
  uint32_t add_material(const MaterialParameters& material);
  uint32_t add_texture(const Texture& texture);
  InterpolatableData<ObjectInstance>& get_interpolatable_object_instances();
  ObjectInstance& get_object_instance(uint32_t id);
  void remove_object_instance(uint32_t id);
  Geometry build_geometry();

private:
  std::shared_ptr<std::vector<Object>> objects;
  InterpolatableData<ObjectInstance> instances;
  std::shared_ptr<std::vector<Material>> materials;
  std::shared_ptr<std::vector<Texture>> textures;
};
