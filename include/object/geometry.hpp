#pragma once
#include <vector>
#include "object/bvh.hpp"
#include "object/material.hpp"
#include "object/object.hpp"
#include "renderer/hit_info.hpp"
#include "renderer/ray.hpp"
#include "util/interpolatable_data.hpp"

class Geometry
{
public:
  Geometry() = default;
  Geometry(const InterpolatableData<Object>& objects, const std::vector<Material>& materials);
  const InterpolatableData<Object>& get_interpolatable_objects() const;
  const std::vector<Material>& get_materials() const;
  bool intersect(const Ray& ray, HitInfo& hit_info) const;

private:
  InterpolatableData<Object> objects;
  std::vector<Material> materials;
  BVH<Object> bvh;
};

Geometry interpolate(const Geometry& a, const Geometry& b, float weight);
