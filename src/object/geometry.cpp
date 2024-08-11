#include "object/geometry.hpp"
#include <vector>
#include "object/object_instance.hpp"
#include "util/interpolatable_data.hpp"

Geometry::Geometry(const std::shared_ptr<const std::vector<Object>>& objects,
                   const InterpolatableData<ObjectInstance>& object_instances,
                   const std::shared_ptr<const std::vector<Material>>& materials,
                   const std::shared_ptr<const std::vector<Texture>>& textures)
  : objects(objects), instances(object_instances), materials(materials), textures(textures), bvh(object_instances.get_data(), 1)
{}

const std::shared_ptr<const std::vector<Object>> Geometry::get_objects() const
{
  return objects;
}

const InterpolatableData<ObjectInstance>& Geometry::get_interpolatable_object_instances() const
{
  return instances;
}

const std::shared_ptr<const std::vector<Material>> Geometry::get_materials() const
{
  return materials;
}

const Material& Geometry::get_material(uint32_t id) const
{
  return (*materials)[id];
}

const std::shared_ptr<const std::vector<Texture>> Geometry::get_textures() const
{
  return textures;
}

const Texture& Geometry::get_texture(uint32_t id) const
{
  return (*textures)[id];
}

bool Geometry::intersect(const Ray& ray, HitInfo& hit_info) const
{
  hit_info.t = ray.config.max_t;
  return (bvh.intersect(ray, hit_info, instances.get_data()));
}

Geometry interpolate(const Geometry& a, const Geometry& b, float weight)
{
  return Geometry(a.get_objects(), interpolate(a.get_interpolatable_object_instances(), b.get_interpolatable_object_instances(), weight), a.get_materials(), a.get_textures());
}
