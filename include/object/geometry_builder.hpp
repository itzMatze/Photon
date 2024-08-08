#pragma once

#include <cstdint>
#include "object/geometry.hpp"
#include "object/material.hpp"
#include "object/object.hpp"
#include "util/interpolatable_data.hpp"

class GeometryBuilder
{
public:
  GeometryBuilder() = default;
  GeometryBuilder(const InterpolatableData<Object>& objects, const std::vector<Material>& materials);

  uint32_t add_object(const Object& object);
  uint32_t add_material(const Material& material);
  InterpolatableData<Object>& get_interpolatable_objects();
  Object& get_object(uint32_t id);
  void remove_object(uint32_t id);
  Geometry build_geometry();

private:
  InterpolatableData<Object> objects;
  std::vector<Material> materials;
};
