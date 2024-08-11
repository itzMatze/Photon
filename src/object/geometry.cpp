#include "object/geometry.hpp"
#include <vector>
#include "util/interpolatable_data.hpp"

Geometry::Geometry(const InterpolatableData<Object>& objects, const std::vector<Material>& materials, const std::vector<Texture>& textures)
  : objects(objects), materials(materials), textures(textures), bvh(objects.get_data(), 1)
{}

const InterpolatableData<Object>& Geometry::get_interpolatable_objects() const
{
  return objects;
}

const std::vector<Material>& Geometry::get_materials() const
{
  return materials;
}

const std::vector<Texture>& Geometry::get_textures() const
{
  return textures;
}

bool Geometry::intersect(const Ray& ray, HitInfo& hit_info) const
{
  hit_info.t = ray.config.max_t;
  return (bvh.intersect(ray, hit_info, objects.get_data()));
}

Geometry interpolate(const Geometry& a, const Geometry& b, float weight)
{
  return Geometry(interpolate(a.get_interpolatable_objects(), b.get_interpolatable_objects(), weight), a.get_materials(), a.get_textures());
}
