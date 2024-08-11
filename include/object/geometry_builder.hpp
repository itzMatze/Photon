#pragma once

#include <cstdint>
#include "object/geometry.hpp"
#include "object/material.hpp"
#include "object/object.hpp"
#include "object/texture.hpp"
#include "util/interpolatable_data.hpp"

class GeometryBuilder
{
public:
  GeometryBuilder() = default;

  uint32_t add_object(const Object& object);
  uint32_t add_material(const Material& material);
  uint32_t add_texture(const Texture& texture);
  uint32_t add_bitmap(const Bitmap& bitmap);
  InterpolatableData<Object>& get_interpolatable_objects();
  Object& get_object(uint32_t id);
  void remove_object(uint32_t id);
  Geometry build_geometry();

private:
  InterpolatableData<Object> objects;
  std::vector<Material> materials;
  std::vector<Texture> textures;
  std::vector<Bitmap> bitmaps;
};
