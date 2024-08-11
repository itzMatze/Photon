#include "object/object.hpp"
#include "object/aabb.hpp"
#include "object/triangle.hpp"
#include "util/spatial_configuration.hpp"
#include "glm/geometric.hpp"

// only triangles are supported
Object::Object(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const SpatialConfiguration& spatial_conf, int32_t material_idx, bool compute_normals) :
  spatial_conf(spatial_conf), vertices(std::make_shared<std::vector<Vertex>>(vertices)), material_idx(material_idx)
{
  assert(material_idx >= 0);
  assert(indices.size() % 3 == 0);
  for (uint32_t i = 0; i < indices.size(); i += 3)
  {
    triangles.emplace_back(indices[i], indices[i + 1], indices[i + 2], this->vertices);
  }
  if (compute_normals)
  {
    for (const auto& triangle : this->triangles) triangle.add_normal_to_vertices(*(this->vertices));
    for (auto& vertex : *(this->vertices))
    {
      // make sure that any normals have been added to the vertex
      if (glm::length(vertex.normal) > 0.0001) vertex.normal = glm::normalize(vertex.normal);
    }
  }
  bvh = BVH<Triangle>(triangles, 1);
}

Object::Object(const std::shared_ptr<std::vector<Vertex>> vertices, const std::vector<Triangle>& triangles, const SpatialConfiguration& spatial_conf, int32_t material_idx) :
  spatial_conf(spatial_conf), vertices(vertices), triangles(triangles), bvh(this->triangles, 1), material_idx(material_idx)
{}

const std::vector<Triangle>& Object::get_triangles() const
{
  return triangles;
}

const std::shared_ptr<std::vector<Vertex>> Object::get_vertices() const
{
  return vertices;
}

AABB Object::get_world_space_bounding_box() const
{
  AABB aabb;
  for (const auto& vertex : *vertices)
  {
    aabb.min = glm::min(spatial_conf.transform_pos(vertex.pos), aabb.min);
    aabb.max = glm::max(spatial_conf.transform_pos(vertex.pos), aabb.max);
  }
  return aabb;
}

const SpatialConfiguration& Object::get_spatial_conf() const
{
  return spatial_conf;
}

SpatialConfiguration& Object::get_spatial_conf()
{
  return spatial_conf;
}

bool Object::intersect(const Ray& ray, HitInfo& hit_info) const
{
  HitInfo cur_hit_info;
  // transform ray into local coordinate system of object
  const Ray transformed_ray(spatial_conf.inverse_transform_pos(ray.origin), spatial_conf.inverse_transform_dir(ray.get_dir()), ray.config);
  // if no intersection was found return false
  if (!bvh.intersect(transformed_ray, cur_hit_info, triangles)) return false;
  // if looking for closest hit check if new intersection is closer than old one
  if (cur_hit_info.t < hit_info.t)
  {
    hit_info = cur_hit_info;
    // transform position and normals
    hit_info.pos = spatial_conf.transform_pos(hit_info.pos);
    hit_info.geometric_normal = glm::normalize(spatial_conf.transform_dir(hit_info.geometric_normal));
    hit_info.normal = glm::normalize(spatial_conf.transform_dir(hit_info.normal));
    hit_info.material_idx = material_idx;
    return true;
  }
  return false;
}

bool Object::intersect(const AABB& aabb) const
{
  const AABB bounding_box = get_world_space_bounding_box();
  return bounding_box.intersect(aabb);
}

Object interpolate(const Object& a, const Object& b, float weight)
{
  SpatialConfiguration spatial_conf = interpolate(a.get_spatial_conf(), b.get_spatial_conf(), weight);
  return Object(a.get_vertices(), a.get_triangles(), spatial_conf, a.material_idx);
}
