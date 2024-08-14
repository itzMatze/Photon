#pragma once

#include <algorithm>
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
  BVH() = default;
  BVH(const std::vector<T>& objects, uint32_t threshold = 1);

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
      if (node.is_leaf)
      {
        // if node is a leaf test the contained objects
        for (uint32_t i = node.indices_offset; i < node.indices_offset + node.indices_count; i++)
        {
          if (objects[indices[i]].intersect(ray, cur_hit_info) && cur_hit_info.t < hit_info.t)
          {
            hit_info = cur_hit_info;
            hit_info.object_id = indices[i];
            if (ray.config.anyhit) return true;
          }
        }
      }
      else if (node.bounding_box.intersect(ray))
      {
        if (node.children.left != -1) nodes_to_test.push(node.children.left);
        if (node.children.right != -1) nodes_to_test.push(node.children.right);
      }
    }
    return hit_info.t < previous_closest;
  }

private:
  struct Node
  {
    Node(int32_t indices_offset, int32_t indices_count, const std::vector<T>& objects, const std::vector<uint32_t>& indices);

    AABB bounding_box;
    bool is_leaf;
    union
    {
      struct
      {
        int32_t left;
        int32_t right;
      } children;
      struct
      {
        int32_t indices_offset;
        int32_t indices_count;
      };
    };
  };

  void init(const std::vector<T>& objects, uint32_t threshold)
  {
    indices = std::vector<uint32_t>(objects.size());
    std::iota(indices.begin(), indices.end(), 0);
    nodes.push_back(BVH<T>::Node(0, objects.size(), objects, indices));
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
        // every node must contain no more than `threshold` many elements
        if (nodes[i].is_leaf && nodes[i].indices_count > threshold)
        {
          if (split(i, axis, objects)) done = false;
        }
      }
      axis = (axis + 1) % 3;
    }
  }

  void apply_split(uint32_t node_idx, const std::vector<T>& objects, std::vector<int8_t>& classifications, uint32_t child_0_count, uint32_t child_1_count, uint32_t common_count)
  {
    const uint32_t total_indices_count = nodes[node_idx].indices_count;
    // distribute the objects as evenly as possible to the children
    // sort indices into buckets: first bucket contains child 0 indices, second common indices, and third child 1 indices
    const uint32_t bucket_offsets[3] = {0, child_0_count, child_0_count + common_count};
    uint32_t bucket_indices[3] = {bucket_offsets[0], bucket_offsets[1], bucket_offsets[2]};
    // sort into buckets using auxiliary vector
    std::vector<uint32_t> auxiliary(total_indices_count);
    for (uint32_t i = 0; i < total_indices_count; i++) auxiliary[bucket_indices[classifications[i]]++] = indices[nodes[node_idx].indices_offset + i];
    // copy back result
    for (uint32_t i = 0; i < total_indices_count; i++) indices[nodes[node_idx].indices_offset + i] = auxiliary[i];
    // split indices evenly and limit splitting point to bucket containing common indices
    const uint32_t split_idx = std::clamp(total_indices_count / 2, bucket_offsets[1], bucket_offsets[2]);
    const uint32_t indices_offset = nodes[node_idx].indices_offset;
    nodes[node_idx].is_leaf = false;
    nodes[node_idx].children.left = -1;
    nodes[node_idx].children.right = -1;
    // create child nodes, their aabb is computed in the constructor
    if (split_idx > 0)
    {
      nodes[node_idx].children.left = nodes.size();
      nodes.emplace_back(indices_offset, split_idx, objects, indices);
    }
    if (split_idx < total_indices_count)
    {
      nodes[node_idx].children.right = nodes.size();
      nodes.emplace_back(indices_offset + split_idx, total_indices_count - split_idx, objects, indices);
    }
  }

  bool split(uint32_t node_idx, uint32_t axis, const std::vector<T>& objects)
  {
    // test all axis until one is found that is able to separate some objects
    // prevent useless nodes and enable detection of endless computation
    // due to all objects being put in the same node all the time caused by floating point errors
    for (uint32_t i = 0; i < 3; i++)
    {
      // store which objects are contained in which child and which objects are contained in both
      // use child_0: 0; common: 1; child_1: 2 for easy indexing when sorting into buckets
      // due to floating point it might happen that an object cannot be assigned to any child
      // assign those to both for even distribution to child nodes
      std::vector<int8_t> classifications(nodes[node_idx].indices_count, 1);
      uint32_t child_0_count = 0;
      uint32_t child_1_count = 0;
      uint32_t common_count = 0;
      // split bounding box into two new ones at the specified axis
      float mid = (nodes[node_idx].bounding_box.max[axis] - nodes[node_idx].bounding_box.min[axis]) / 2.0;
      glm::vec3 p0 = nodes[node_idx].bounding_box.min;
      p0[axis] += mid;
      glm::vec3 p1 = nodes[node_idx].bounding_box.max;
      p1[axis] -= mid;
      AABB box_0(nodes[node_idx].bounding_box.min, p1);
      AABB box_1(p0, nodes[node_idx].bounding_box.max);
      for (uint32_t i = 0; i < nodes[node_idx].indices_count; i++)
      {
        const uint32_t object_idx = indices[nodes[node_idx].indices_offset + i];
        const bool intersect_0 = objects[object_idx].intersect(box_0);
        const bool intersect_1 = objects[object_idx].intersect(box_1);
        if (intersect_0 && intersect_1)
        {
          classifications[i] = 1;
          common_count++;
        }
        else if (intersect_0)
        {
          classifications[i] = 0;
          child_0_count++;
        }
        else if (intersect_1)
        {
          classifications[i] = 2;
          child_1_count++;
        }
      }
      if ((child_0_count > 0 && child_1_count > 0) || common_count > 0)
      {
        apply_split(node_idx, objects, classifications, child_0_count, child_1_count, common_count);
        return true;
      }
      axis = (axis + 1) % 3;
    }
    return false;
  }

  std::vector<Node> nodes;
  std::vector<uint32_t> indices;
};
