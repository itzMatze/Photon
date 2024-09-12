#include "object/object_instance.hpp"
#include "util/spatial_configuration.hpp"

ObjectInstance::ObjectInstance(std::shared_ptr<const std::vector<Object>> objects, uint32_t object_id, int32_t material_id, bool is_emissive, const SpatialConfiguration& spatial_conf)
  : objects(objects), object_id(object_id), material_id(material_id), is_emissive(is_emissive), spatial_conf(spatial_conf)
{}

AABB ObjectInstance::get_world_space_bounding_box() const
{
  AABB aabb;
  for (const auto& vertex : *((*objects)[object_id].get_vertices()))
  {
    aabb.min = glm::min(spatial_conf.transform_pos(vertex.pos), aabb.min);
    aabb.max = glm::max(spatial_conf.transform_pos(vertex.pos), aabb.max);
  }
  return aabb;
}

const std::shared_ptr<const std::vector<Object>> ObjectInstance::get_objects() const
{
  return objects;
}

const Object& ObjectInstance::get_object() const
{
  return (*objects)[object_id];
}

const SpatialConfiguration& ObjectInstance::get_spatial_conf() const
{
  return spatial_conf;
}

SpatialConfiguration& ObjectInstance::get_spatial_conf()
{
  return spatial_conf;
}

bool ObjectInstance::intersect(const Ray& ray, HitInfo& hit_info) const
{
  HitInfo cur_hit_info;
  // transform ray into local coordinate system of object
  const Ray transformed_ray(spatial_conf.inverse_transform_pos(ray.origin), spatial_conf.inverse_transform_dir(ray.get_dir()), ray.config);
  (*objects)[object_id].intersect(transformed_ray, cur_hit_info);
  // if looking for closest hit check if new intersection is closer than old one
  if (cur_hit_info.t < hit_info.t)
  {
    hit_info = cur_hit_info;
    // transform position and normals
    hit_info.pos = spatial_conf.transform_pos(hit_info.pos);
    hit_info.geometric_normal = glm::normalize(spatial_conf.transform_dir(hit_info.geometric_normal));
    hit_info.normal = glm::normalize(spatial_conf.transform_dir(hit_info.normal));
    if (material_id >= 0) hit_info.material_id = material_id;
    return true;
  }
  return false;
}

bool ObjectInstance::intersect(const AABB& aabb) const
{
  const AABB bounding_box = get_world_space_bounding_box();
  return bounding_box.intersect(aabb);
}

ObjectInstance interpolate(const ObjectInstance& a, const ObjectInstance& b, float weight)
{
  SpatialConfiguration spatial_conf = interpolate(a.get_spatial_conf(), b.get_spatial_conf(), weight);
  return ObjectInstance(a.get_objects(), a.object_id, a.material_id, a.is_emissive, spatial_conf);
}
