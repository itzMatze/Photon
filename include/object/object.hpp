#pragma once

#include <memory>
#include <vector>
#include "object/bvh.hpp"
#include "object/mesh.hpp"
#include "object/triangle.hpp"
#include "object/vertex.hpp"

class Object
{
public:
  Object() = default;
  Object(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, bool compute_normals = false);
  Object(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::vector<Mesh>& meshes, bool compute_normals = false);
  const std::vector<Triangle>& get_triangles() const;
  const std::shared_ptr<std::vector<Vertex>> get_vertices() const;
  bool intersect(const Ray& ray, HitInfo& hit_info) const;

private:
  std::shared_ptr<std::vector<Vertex>> vertices;
  std::vector<Triangle> triangles;
  std::vector<Mesh> meshes;
  BVH<Triangle> bvh;

  void init(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, bool compute_normals);
};
