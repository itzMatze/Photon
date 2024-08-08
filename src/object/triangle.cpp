#include "object/triangle.hpp"

#define EPSILON 0.0001

Triangle::Triangle(uint32_t idx0, uint32_t idx1, uint32_t idx2, const std::shared_ptr<const std::vector<Vertex>> vertices) : vertices(vertices)
{
  vertex_indices[0] = idx0;
  vertex_indices[1] = idx1;
  vertex_indices[2] = idx2;
  geometric_normal = cm::normalize(cm::cross(cm::normalize((*vertices)[idx1].pos - (*vertices)[idx0].pos), cm::normalize((*vertices)[idx2].pos - (*vertices)[idx0].pos)));
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
  const float dot_n_dir = cm::dot(ray.get_dir(), geometric_normal);
  // backface culling
  if (ray.config.backface_culling && dot_n_dir >= 0.0) return false;
  hit_info.t = (cm::dot(geometric_normal, v0.pos) - cm::dot(ray.origin, geometric_normal)) / dot_n_dir;
  // hit is behind the ray or outside of allowed range
  if (hit_info.t < 0.0 || hit_info.t > ray.config.max_t) return false;
  hit_info.pos = ray.at(hit_info.t);
  // check if point lies inside or outside of the triangle
  {
    const cm::Vec3 e = cm::normalize(v1.pos - v0.pos);
    const cm::Vec3 vp = cm::normalize(hit_info.pos - v0.pos);
    if (cm::dot(geometric_normal, cm::cross(e, vp)) < -EPSILON) return false;
  }
  {
    const cm::Vec3 e = cm::normalize(v2.pos - v1.pos);
    const cm::Vec3 vp = cm::normalize(hit_info.pos - v1.pos);
    if (cm::dot(geometric_normal, cm::cross(e, vp)) < -EPSILON) return false;
  }
  {
    const cm::Vec3 e = cm::normalize(v0.pos - v2.pos);
    const cm::Vec3 vp = cm::normalize(hit_info.pos - v2.pos);
    if (cm::dot(geometric_normal, cm::cross(e, vp)) < -EPSILON) return false;
  }
  hit_info.bary.u = cm::length(cm::cross(hit_info.pos - v0.pos, v2.pos - v0.pos)) / cm::length(cm::cross(v1.pos - v0.pos, v2.pos - v0.pos));
  hit_info.bary.v = cm::length(cm::cross(hit_info.pos - v0.pos, v1.pos - v0.pos)) / cm::length(cm::cross(v1.pos - v0.pos, v2.pos - v0.pos));
  hit_info.geometric_normal = geometric_normal;
  hit_info.normal = cm::normalize(hit_info.bary.u * v1.normal + hit_info.bary.v * v2.normal + (1.0 - hit_info.bary.u - hit_info.bary.v) * v0.normal);
  hit_info.tex_coords = hit_info.bary.u * v1.tex_coords + hit_info.bary.v * v2.tex_coords + (1.0 - hit_info.bary.u - hit_info.bary.v) * v0.tex_coords;
  return true;
}

AABB Triangle::get_bounding_box() const
{
  AABB bounding_box;
  bounding_box.min = cm::min((*vertices)[vertex_indices[0]].pos, cm::min((*vertices)[vertex_indices[1]].pos, (*vertices)[vertex_indices[2]].pos));
  bounding_box.max = cm::max((*vertices)[vertex_indices[0]].pos, cm::max((*vertices)[vertex_indices[1]].pos, (*vertices)[vertex_indices[2]].pos));
  return bounding_box;
}

bool triangle_aabb_intersection(const AABB& aabb, const cm::Vec3& v0, const cm::Vec3& v1, const cm::Vec3& v2)
{
  cm::Vec3 box_half_size = (aabb.max - aabb.min) / 2.0;
  cm::Vec3 box_center = aabb.min + box_half_size;

  // move everything so that the boxcenter is in (0,0,0)
  cm::Vec3 centered_v0 = v0 - box_center;
  cm::Vec3 centered_v1 = v1 - box_center;
  cm::Vec3 centered_v2 = v2 - box_center;

  // first, test triangle aabb against aabb
  cm::Vec3 min_v = cm::min(centered_v0, cm::min(centered_v1, centered_v2));
  cm::Vec3 max_v = cm::max(centered_v0, cm::max(centered_v1, centered_v2));
  if(min_v.x > box_half_size.x || max_v.x < -box_half_size.x) return false;
  if(min_v.y > box_half_size.y || max_v.y < -box_half_size.y) return false;
  if(min_v.z > box_half_size.z || max_v.z < -box_half_size.z) return false;

  // second, test triangle plane against aabb
  // compute triangle edges
  cm::Vec3 e0 = centered_v1 - centered_v0;
  cm::Vec3 e1 = centered_v2 - centered_v1;
  cm::Vec3 e2 = centered_v0 - centered_v2;

  cm::Vec3 normal = cm::cross(e0, e1);
  cm::Vec3 signs = cm::sign(normal);
  cm::Vec3 vmin = -signs * box_half_size;
  cm::Vec3 vmax = signs * box_half_size;
  float d = -cm::dot(normal, centered_v0);
  if(cm::dot(normal, vmin) + d > 0.0f) return false;
  if(cm::dot(normal, vmax) + d < 0.0f) return false;

  // third, test triangle against aabb
  const auto axis_test = [&](const cm::Vec3& a) -> bool
  {
    const float p0 = cm::dot(a, centered_v0);
    const float p1 = cm::dot(a, centered_v1);
    const float p2 = cm::dot(a, centered_v2);
    const float rad = cm::dot(box_half_size, cm::abs(a));
    if(std::min(p0, std::min(p1, p2)) > rad || std::max(p0, std::max(p1, p2)) < -rad) return false;
    return true;
  };
  if (!axis_test(cross(cm::Vec3(1.0, 0.0, 0.0), e0))) return false;
  if (!axis_test(cross(cm::Vec3(1.0, 0.0, 0.0), e1))) return false;
  if (!axis_test(cross(cm::Vec3(1.0, 0.0, 0.0), e2))) return false;
  if (!axis_test(cross(cm::Vec3(0.0, 1.0, 0.0), e0))) return false;
  if (!axis_test(cross(cm::Vec3(0.0, 1.0, 0.0), e1))) return false;
  if (!axis_test(cross(cm::Vec3(0.0, 1.0, 0.0), e2))) return false;
  if (!axis_test(cross(cm::Vec3(0.0, 0.0, 1.0), e0))) return false;
  if (!axis_test(cross(cm::Vec3(0.0, 0.0, 1.0), e1))) return false;
  if (!axis_test(cross(cm::Vec3(0.0, 0.0, 1.0), e2))) return false;

  return true;
}

bool Triangle::intersect(const AABB& aabb, bool accurate) const
{
  return triangle_aabb_intersection(aabb, (*vertices)[vertex_indices[0]].pos, (*vertices)[vertex_indices[1]].pos, (*vertices)[vertex_indices[2]].pos);
}
