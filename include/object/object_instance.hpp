#pragma once

#include "object/aabb.hpp"
#include "object/object.hpp"
#include "renderer/hit_info.hpp"
#include "util/spatial_configuration.hpp"

class ObjectInstance
{
public:
  ObjectInstance(std::shared_ptr<const std::vector<Object>> objects, uint32_t object_id, int32_t material_id, bool is_emissive, const SpatialConfiguration& spatial_conf);
  AABB get_world_space_bounding_box() const;
  const std::shared_ptr<const std::vector<Object>> get_objects() const;
  const Object& get_object() const;
  const SpatialConfiguration& get_spatial_conf() const;
  SpatialConfiguration& get_spatial_conf();
  void set_spatial_conf(const SpatialConfiguration& spatial_configuration);
  bool intersect(const Ray& ray, HitInfo& hit_info) const;
  bool intersect(const AABB& aabb) const;

  uint32_t object_id;
  int32_t material_id;
  bool is_emissive;

private:
  std::shared_ptr<const std::vector<Object>> objects;
  SpatialConfiguration spatial_conf;
};

ObjectInstance interpolate(const ObjectInstance& a, const ObjectInstance& b, float weight);
