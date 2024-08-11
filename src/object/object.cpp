#include "object/object.hpp"
#include "object/triangle.hpp"
#include "util/spatial_configuration.hpp"
#include "glm/geometric.hpp"

// only triangles are supported
Object::Object(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, bool compute_normals) :
  vertices(std::make_shared<std::vector<Vertex>>(vertices))
{
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
  vertices(vertices), triangles(triangles), bvh(this->triangles, 1)
{}

const std::vector<Triangle>& Object::get_triangles() const
{
  return triangles;
}

const std::shared_ptr<std::vector<Vertex>> Object::get_vertices() const
{
  return vertices;
}

bool Object::intersect(const Ray& ray, HitInfo& hit_info) const
{
  // if no intersection was found return false
  if (!bvh.intersect(ray, hit_info, triangles)) return false;
  return true;
}
