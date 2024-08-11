#include "object/geometry_builder.hpp"

uint32_t GeometryBuilder::add_object(const Object& object)
{
  return objects.add_new_data(object);
}

uint32_t GeometryBuilder::add_material(const Material& material)
{
  materials.emplace_back(material);
  return materials.size() - 1;
}

uint32_t GeometryBuilder::add_texture(const Texture& texture)
{
  textures.emplace_back(texture);
  return textures.size() - 1;
}

InterpolatableData<Object>& GeometryBuilder::get_interpolatable_objects()
{
  return objects;
}

Object& GeometryBuilder::get_object(uint32_t id)
{
  return objects.get_element(id);
}

void GeometryBuilder::remove_object(uint32_t id)
{
  objects.remove_element(id);
}

Geometry GeometryBuilder::build_geometry()
{
  return Geometry(objects, materials, textures);
}
