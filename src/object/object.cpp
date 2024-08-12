#include "object/object.hpp"
#include "object/triangle.hpp"
#include "glm/geometric.hpp"

// only triangles are supported
Object::Object(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, bool compute_normals) :
  vertices(std::make_shared<std::vector<Vertex>>(vertices))
{
  init(vertices, indices, compute_normals);
}

Object::Object(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::vector<Mesh>& meshes, bool compute_normals) :
  vertices(std::make_shared<std::vector<Vertex>>(vertices)), meshes(meshes)
{
  init(vertices, indices, compute_normals);
}

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
  for (const auto& mesh : meshes)
  {
    if (hit_info.object_id >= mesh.triangle_index_offset && hit_info.object_id < (mesh.triangle_index_offset + mesh.triangle_index_count))
    {
      if (mesh.material_id >= 0) hit_info.material_id = mesh.material_id;
      return true;
    }
  }
  return true;
}

void Object::init(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, bool compute_normals)
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
