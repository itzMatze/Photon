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

template<> BVH<ObjectInstance>::Node::Node(const std::vector<uint32_t>& indices, const std::vector<ObjectInstance>& object_instances) : is_leaf(true), indices(indices)
{
  for (uint32_t i : indices)
  {
    bounding_box.min = glm::min(bounding_box.min, object_instances[i].get_world_space_bounding_box().min);
    bounding_box.max = glm::max(bounding_box.max, object_instances[i].get_world_space_bounding_box().max);
  }
}

template<> BVH<Triangle>::BVH(const std::vector<Triangle>& objects, uint32_t threshold)
{
  spdlog::debug("Creating BLAS of {} triangles with threshold of {}", objects.size(), threshold);
  init(objects, threshold);
  spdlog::debug("Successfully created BLAS with {} nodes", nodes.size());
}

template<> BVH<Triangle>::Node::Node(const std::vector<uint32_t>& indices, const std::vector<Triangle>& triangles) : is_leaf(true), indices(indices)
{
  for (uint32_t i : indices)
  {
    for (uint32_t v = 0; v < 3; v++)
    {
      bounding_box.min = glm::min(bounding_box.min, triangles[i].get_triangle_vertex(v).pos);
      bounding_box.max = glm::max(bounding_box.max, triangles[i].get_triangle_vertex(v).pos);
    }
  }
}
