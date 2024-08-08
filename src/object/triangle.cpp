#include "object/triangle.hpp"
#include "glm/geometric.hpp"

#define EPSILON 0.0001

Triangle::Triangle(uint32_t idx0, uint32_t idx1, uint32_t idx2, const std::shared_ptr<const std::vector<Vertex>> vertices) : vertices(vertices)
{
  vertex_indices[0] = idx0;
  vertex_indices[1] = idx1;
  vertex_indices[2] = idx2;
  geometric_normal = glm::normalize(glm::cross(glm::normalize((*vertices)[idx1].pos - (*vertices)[idx0].pos), glm::normalize((*vertices)[idx2].pos - (*vertices)[idx0].pos)));
}

void Triangle::add_normal_to_vertices(std::vector<Vertex>& vertices) const
{
  vertices[vertex_indices[0]].normal += geometric_normal;
  vertices[vertex_indices[1]].normal += geometric_normal;
  vertices[vertex_indices[2]].normal += geometric_normal;
}

const Vertex& Triangle::get_triangle_vertex(uint32_t idx) const
{
  return (*vertices)[vertex_indices[idx]];
}

bool Triangle::intersect(const Ray& ray, HitInfo& hit_info) const
{
  const Vertex& v0 = (*vertices)[vertex_indices[0]];
  const Vertex& v1 = (*vertices)[vertex_indices[1]];
  const Vertex& v2 = (*vertices)[vertex_indices[2]];
  const float dot_n_dir = glm::dot(ray.get_dir(), geometric_normal);
  // backface culling
  if (ray.config.backface_culling && dot_n_dir >= 0.0) return false;
  hit_info.t = (glm::dot(geometric_normal, v0.pos) - glm::dot(ray.origin, geometric_normal)) / dot_n_dir;
  // hit is behind the ray or outside of allowed range
  if (hit_info.t < 0.0 || hit_info.t > ray.config.max_t) return false;
  hit_info.pos = ray.at(hit_info.t);
  // check if point lies inside or outside of the triangle
  {
    const glm::vec3 e = glm::normalize(v1.pos - v0.pos);
    const glm::vec3 vp = glm::normalize(hit_info.pos - v0.pos);
    if (glm::dot(geometric_normal, glm::cross(e, vp)) < -EPSILON) return false;
  }
  {
    const glm::vec3 e = glm::normalize(v2.pos - v1.pos);
    const glm::vec3 vp = glm::normalize(hit_info.pos - v1.pos);
    if (glm::dot(geometric_normal, glm::cross(e, vp)) < -EPSILON) return false;
  }
  {
    const glm::vec3 e = glm::normalize(v0.pos - v2.pos);
    const glm::vec3 vp = glm::normalize(hit_info.pos - v2.pos);
    if (glm::dot(geometric_normal, glm::cross(e, vp)) < -EPSILON) return false;
  }
  hit_info.bary.s = glm::length(glm::cross(hit_info.pos - v0.pos, v2.pos - v0.pos)) / glm::length(glm::cross(v1.pos - v0.pos, v2.pos - v0.pos));
  hit_info.bary.t = glm::length(glm::cross(hit_info.pos - v0.pos, v1.pos - v0.pos)) / glm::length(glm::cross(v1.pos - v0.pos, v2.pos - v0.pos));
  hit_info.geometric_normal = geometric_normal;
  hit_info.normal = glm::normalize(hit_info.bary.s * v1.normal + hit_info.bary.t * v2.normal + (1.0f - hit_info.bary.s - hit_info.bary.t) * v0.normal);
  hit_info.tex_coords = hit_info.bary.s * v1.tex_coords + hit_info.bary.t * v2.tex_coords + (1.0f - hit_info.bary.s - hit_info.bary.t) * v0.tex_coords;
  return true;
}

AABB Triangle::get_bounding_box() const
{
  AABB bounding_box;
  bounding_box.min = glm::min((*vertices)[vertex_indices[0]].pos, glm::min((*vertices)[vertex_indices[1]].pos, (*vertices)[vertex_indices[2]].pos));
  bounding_box.max = glm::max((*vertices)[vertex_indices[0]].pos, glm::max((*vertices)[vertex_indices[1]].pos, (*vertices)[vertex_indices[2]].pos));
  return bounding_box;
}

bool triangle_aabb_intersection(const AABB& aabb, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2)
{
  glm::vec3 box_half_size = (aabb.max - aabb.min) / 2.0f;
  glm::vec3 box_center = aabb.min + box_half_size;

  // move everything so that the boxcenter is in (0,0,0)
  glm::vec3 centered_v0 = v0 - box_center;
  glm::vec3 centered_v1 = v1 - box_center;
  glm::vec3 centered_v2 = v2 - box_center;

  // first, test triangle aabb against aabb
  glm::vec3 min_v = glm::min(centered_v0, glm::min(centered_v1, centered_v2));
  glm::vec3 max_v = glm::max(centered_v0, glm::max(centered_v1, centered_v2));
  if(min_v.x > box_half_size.x || max_v.x < -box_half_size.x) return false;
  if(min_v.y > box_half_size.y || max_v.y < -box_half_size.y) return false;
  if(min_v.z > box_half_size.z || max_v.z < -box_half_size.z) return false;

  // second, test triangle plane against aabb
  // compute triangle edges
  glm::vec3 e0 = centered_v1 - centered_v0;
  glm::vec3 e1 = centered_v2 - centered_v1;
  glm::vec3 e2 = centered_v0 - centered_v2;

  glm::vec3 normal = glm::cross(e0, e1);
  glm::vec3 signs = glm::sign(normal);
  glm::vec3 vmin = -signs * box_half_size;
  glm::vec3 vmax = signs * box_half_size;
  float d = -glm::dot(normal, centered_v0);
  if(glm::dot(normal, vmin) + d > 0.0f) return false;
  if(glm::dot(normal, vmax) + d < 0.0f) return false;

  // third, test triangle against aabb
  const auto axis_test = [&](const glm::vec3& a) -> bool
  {
    const float p0 = glm::dot(a, centered_v0);
    const float p1 = glm::dot(a, centered_v1);
    const float p2 = glm::dot(a, centered_v2);
    const float rad = glm::dot(box_half_size, glm::abs(a));
    if(std::min(p0, std::min(p1, p2)) > rad || std::max(p0, std::max(p1, p2)) < -rad) return false;
    return true;
  };
  if (!axis_test(cross(glm::vec3(1.0, 0.0, 0.0), e0))) return false;
  if (!axis_test(cross(glm::vec3(1.0, 0.0, 0.0), e1))) return false;
  if (!axis_test(cross(glm::vec3(1.0, 0.0, 0.0), e2))) return false;
  if (!axis_test(cross(glm::vec3(0.0, 1.0, 0.0), e0))) return false;
  if (!axis_test(cross(glm::vec3(0.0, 1.0, 0.0), e1))) return false;
  if (!axis_test(cross(glm::vec3(0.0, 1.0, 0.0), e2))) return false;
  if (!axis_test(cross(glm::vec3(0.0, 0.0, 1.0), e0))) return false;
  if (!axis_test(cross(glm::vec3(0.0, 0.0, 1.0), e1))) return false;
  if (!axis_test(cross(glm::vec3(0.0, 0.0, 1.0), e2))) return false;

  return true;
}

bool Triangle::intersect(const AABB& aabb, bool accurate) const
{
  return triangle_aabb_intersection(aabb, (*vertices)[vertex_indices[0]].pos, (*vertices)[vertex_indices[1]].pos, (*vertices)[vertex_indices[2]].pos);
}
