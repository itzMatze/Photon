#pragma once

#include <cstdint>
#include <numeric>
#include <stack>
#include <vector>
#include "object/aabb.hpp"
#include "renderer/hit_info.hpp"
#include "util/vec3.hpp"

template<typename T>
class BVH
{
public:
  struct Node
  {
    Node(const std::vector<uint32_t>& indices, const std::vector<T>& objects);

    AABB bounding_box;
    bool is_leaf;
    struct
    {
      int32_t left;
      int32_t right;
    } children;
    std::vector<uint32_t> indices;
  };

  BVH() = default;
  BVH(const std::vector<T>& objects, uint32_t threshold = 1)
  {
    std::vector<uint32_t> indices(objects.size());
    std::iota(indices.begin(), indices.end(), 0);
    nodes.push_back(BVH<T>::Node(indices, objects));
    bool done = false;
    uint32_t axis = 0;
    // split nodes until termination criterium is fullfilled
    while (!done)
    {
      done = true;
      // nodes vector is edited when splitting a node which invalidates the iterators
      uint32_t node_count = nodes.size();
      for (uint32_t i = 0; i < node_count; i++)
      {
        Node& node = nodes[i];
        // every node must contain no more than `threshold` many elements
        if (node.indices.size() > threshold)
        {
          done = false;
          split(node, axis, objects);
        }
      }
      axis = (axis + 1) % 3;
    }
  }

  void split(BVH<T>::Node& node, uint32_t axis, const std::vector<T>& objects)
  {
    // split bounding box into two new ones at the specified axis
    float mid = (node.bounding_box.max[axis] - node.bounding_box.min[axis]) / 2.0;
    cm::Vec3 p0 = node.bounding_box.min;
    p0[axis] += mid;
    cm::Vec3 p1 = node.bounding_box.max;
    p1[axis] -= mid;
    AABB box_0(node.bounding_box.min, p1);
    AABB box_1(p0, node.bounding_box.max);
    // store which objects are contained in which child and which objects are contained in both
    std::vector<uint32_t> indices_0;
    std::vector<uint32_t> indices_1;
    std::vector<uint32_t> common_indices;
    for (uint32_t i : node.indices)
    {
      bool intersect_0 = objects[i].intersect(box_0);
      bool intersect_1 = objects[i].intersect(box_1);
      if (intersect_0 && intersect_1) common_indices.push_back(i);
      else if (intersect_0) indices_0.push_back(i);
      else if (intersect_1) indices_1.push_back(i);
    }
    // distribute the objects as evenly as possible to the children
    int32_t diff = indices_0.size() - indices_1.size();
    uint32_t common_idx = 0;
    // fill up child with fewer elements
    while (diff > 0 && common_idx < common_indices.size())
    {
      indices_1.push_back(common_indices[common_idx]);
      diff--;
      common_idx++;
    }
    while (diff < 0 && common_idx < common_indices.size())
    {
      indices_0.push_back(common_indices[common_idx]);
      diff++;
      common_idx++;
    }
    // distribute the remaining objects alternatingly
    while (common_idx < common_indices.size())
    {
      if (diff & 1u) indices_0.push_back(common_indices[common_idx]);
      else indices_1.push_back(common_indices[common_idx]);
      diff++;
      common_idx++;
    }
    // create child nodes, their aabb is computed in the constructor
    node.is_leaf = false;
    node.indices.clear();
    node.children.left = nodes.size();
    node.children.right = nodes.size() + 1;
    if (indices_0.size() > 0) nodes.emplace_back(indices_0, objects);
    if (indices_1.size() > 0) nodes.emplace_back(indices_1, objects);
  }

  bool intersect(const Ray& ray, HitInfo& hit_info, const std::vector<T>& objects) const
  {
    const float previous_closest = hit_info.t;
    std::stack<int32_t> nodes_to_test;
    // add root node as initial node
    nodes_to_test.push(0);
    HitInfo cur_hit_info;
    while (!nodes_to_test.empty())
    {
      const Node& node = nodes[nodes_to_test.top()];
      nodes_to_test.pop();
      if (!node.is_leaf && node.bounding_box.intersect(ray))
      {
        nodes_to_test.push(node.children.left);
        nodes_to_test.push(node.children.right);
      }
      else
      {
        // if node is a leaf test the contained objects
        for (const uint32_t idx : node.indices)
        {
          if (objects[idx].intersect(ray, cur_hit_info) && cur_hit_info.t < hit_info.t)
          {
            hit_info = cur_hit_info;
            if (ray.config.anyhit) return true;
          }
        }
      }
    }
    return hit_info.t < previous_closest;
  }

  std::vector<Node> nodes;
};
