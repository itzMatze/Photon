#include "object/object.hpp"
#include "object/triangle.hpp"
#include "glm/geometric.hpp"
#include "util/log.hpp"

// only triangles are supported
Object::Object(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, bool compute_normals) :
  vertices(std::make_shared<std::vector<Vertex>>(vertices))
{
  init(vertices, indices, compute_normals);
}

Object::Object(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::vector<Mesh>& meshes, const std::vector<Mesh>& emissive_meshes, bool compute_normals) :
  vertices(std::make_shared<std::vector<Vertex>>(vertices)), meshes(meshes), emissive_meshes(emissive_meshes)
{
  init(vertices, indices, compute_normals);
}

const std::shared_ptr<std::vector<Vertex>> Object::get_vertices() const
{
  return vertices;
}

const std::vector<Triangle>& Object::get_triangles() const
{
  return triangles;
}

const std::vector<Mesh>& Object::get_meshes() const
{
  return meshes;
}

const std::vector<Mesh>& Object::get_emissive_meshes() const
{
  return emissive_meshes;
}

bool Object::intersect(const Ray& ray, HitInfo& hit_info) const
{
  // if no intersection was found return false
  if (!bvh.intersect(ray, hit_info, triangles)) return false;
  const auto contains_triangle = [](const HitInfo& hit_info, const Mesh& mesh) -> bool {
    return (hit_info.object_id >= mesh.triangle_index_offset && hit_info.object_id < (mesh.triangle_index_offset + mesh.triangle_index_count));
  };
  for (const auto& mesh : meshes)
  {
    if (contains_triangle(hit_info, mesh))
    {
      if (mesh.material_id >= 0) hit_info.material_id = mesh.material_id;
      return true;
    }
  }
  for (const auto& mesh : emissive_meshes)
  {
    if (contains_triangle(hit_info, mesh))
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
  spdlog::debug("Creating object with {} vertices and {} triangles", vertices.size(), indices.size() / 3);
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
  spdlog::debug("Successfully created object");
}
