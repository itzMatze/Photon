#include "scene/geometry_builder.hpp"
#include <memory>
#include "material/material.hpp"
#include "object/object_instance.hpp"
#include "util/spatial_configuration.hpp"

GeometryBuilder::GeometryBuilder() : objects(std::make_shared<std::vector<Object>>()), materials(std::make_shared<std::vector<Material>>()), textures(std::make_shared<std::vector<Texture>>())
{}

uint32_t GeometryBuilder::add_object(const Object& object)
{
  objects->emplace_back(object);
  return objects->size() - 1;
}

uint32_t GeometryBuilder::add_object_instance(uint32_t object_id, int32_t material_id, const SpatialConfiguration& spatial_conf)
{
  // if instance has an override material check if it is emissive
  bool is_emissive = material_id > -1 ? (*materials)[material_id].is_emissive() : false;
  return instances.add_new_data(ObjectInstance(objects, object_id, material_id, is_emissive, spatial_conf));
}

uint32_t GeometryBuilder::add_material(const MaterialParameters& material)
{
  materials->emplace_back(Material(material, textures));
  return materials->size() - 1;
}

uint32_t GeometryBuilder::add_texture(const Texture& texture)
{
  textures->emplace_back(texture);
  return textures->size() - 1;
}

InterpolatableData<ObjectInstance>& GeometryBuilder::get_interpolatable_object_instances()
{
  return instances;
}

ObjectInstance& GeometryBuilder::get_object_instance(uint32_t id)
{
  return instances.get_element(id);
}

void GeometryBuilder::remove_object_instance(uint32_t id)
{
  instances.remove_element(id);
}

Geometry GeometryBuilder::build_geometry()
{
  return Geometry(objects, instances, materials, textures);
}
