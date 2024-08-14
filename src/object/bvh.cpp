#include "object/bvh.hpp"
#include "object/object_instance.hpp"
#include "object/triangle.hpp"
#include "util/log.hpp"

template<> BVH<ObjectInstance>::BVH(const std::vector<ObjectInstance>& objects, uint32_t threshold)
{
  spdlog::debug("Creating TLAS of {} object instances with threshold of {}", objects.size(), threshold);
  init(objects, threshold);
  spdlog::debug("Successfully created TLAS with {} nodes", nodes.size());
}

template<> BVH<ObjectInstance>::Node::Node(int32_t indices_offset, int32_t indices_count, const std::vector<ObjectInstance>& object_instances, const std::vector<uint32_t>& indices)
  : is_leaf(true), indices_offset(indices_offset), indices_count(indices_count)
{
  for (uint32_t i = indices_offset; i < indices_offset + indices_count; i++)
  {
    bounding_box.min = glm::min(bounding_box.min, object_instances[indices[i]].get_world_space_bounding_box().min);
    bounding_box.max = glm::max(bounding_box.max, object_instances[indices[i]].get_world_space_bounding_box().max);
  }
  // add bias so we do not miss a bounding box due to floating point
  bounding_box.min -= 0.001f;
  bounding_box.max += 0.001f;
}

template<> BVH<Triangle>::BVH(const std::vector<Triangle>& objects, uint32_t threshold)
{
  spdlog::debug("Creating BLAS of {} triangles with threshold of {}", objects.size(), threshold);
  init(objects, threshold);
  spdlog::debug("Successfully created BLAS with {} nodes", nodes.size());
}

template<> BVH<Triangle>::Node::Node(int32_t indices_offset, int32_t indices_count, const std::vector<Triangle>& triangles, const std::vector<uint32_t>& indices)
  : is_leaf(true), indices_offset(indices_offset), indices_count(indices_count)
{
  for (uint32_t i = indices_offset; i < indices_offset + indices_count; i++)
  {
    for (uint32_t v = 0; v < 3; v++)
    {
      bounding_box.min = glm::min(bounding_box.min, triangles[indices[i]].get_triangle_vertex(v).pos);
      bounding_box.max = glm::max(bounding_box.max, triangles[indices[i]].get_triangle_vertex(v).pos);
    }
  }
  // add bias so we do not miss a bounding box due to floating point
  bounding_box.min -= 0.001f;
  bounding_box.max += 0.001f;
}
