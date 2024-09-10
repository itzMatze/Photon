#pragma once
#include <memory>
#include <vector>
#include "object/aabb.hpp"
#include "object/vertex.hpp"
#include "renderer/hit_info.hpp"
#include "renderer/ray.hpp"
#include "util/vec3.hpp"

class Triangle
{
public:
  Triangle(uint32_t idx0, uint32_t idx1, uint32_t idx2, const std::shared_ptr<const std::vector<Vertex>> vertices);
  void add_normal_to_vertices(std::vector<Vertex>& vertices) const;
  const Vertex& get_triangle_vertex(uint32_t idx) const;
  AABB get_bounding_box() const;
  glm::vec3 get_geometric_normal() const;

  bool intersect(const Ray& ray, HitInfo& hit_info) const;
  bool intersect(const AABB& aabb, bool accurate = true) const;

private:
  const std::shared_ptr<const std::vector<Vertex>> vertices;
  uint32_t vertex_indices[3];
  glm::vec3 geometric_normal;
};

