#include "scene/scene_file_handler.hpp"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <memory>

#include "rapidjson/document.h"

#include "image/image_file_handler.hpp"
#include "object/material.hpp"
#include "scene/scene_builder.hpp"

int load_file(const std::string& path, rapidjson::Document& document)
{
  std::ifstream file(path, std::ios::binary);
  if (!file.is_open())
  {
    std::cerr << "Failed to open file: " << path << std::endl;
    return 1;
  };
  std::stringstream file_stream;
  file_stream << file.rdbuf();
  std::string file_content = file_stream.str();

  document.Parse(file_content.c_str());
  if (document.HasParseError())
  {
    std::cerr << "Failed to parse file: " << path << std::endl;
    return 2;
  }
  return 0;
}

glm::vec3 get_vec3(const rapidjson::GenericValue<rapidjson::UTF8<>>& rj_vec3, int base_index = 0)
{
  return glm::vec3(rj_vec3[base_index].GetFloat(), rj_vec3[base_index + 1].GetFloat(), rj_vec3[base_index + 2].GetFloat());
}

void load_lights(const auto& rj_lights, SceneBuilder& scene_builder)
{
  for (const auto& light : rj_lights)
  {
    float intensity = light["intensity"].GetFloat();
    glm::vec3 position = get_vec3(light["position"]);
    scene_builder.get_lights().add_new_data(Light(intensity, position));
  }
}

void load_textures(const auto& rj_textures, SceneBuilder& scene_builder)
{
  for (const auto& texture : rj_textures)
  {
    const std::string texture_name = texture.GetString();
    const std::string path("assets/textures/" + texture_name);
    scene_builder.get_geometry().add_texture(load_image(path));
  }
}

void load_materials(const auto& rj_materials, SceneBuilder& scene_builder)
{
  for (const auto& material : rj_materials)
  {
    MaterialType type;
    if (std::string("diffuse") == material["type"].GetString()) type = MaterialType::Diffuse;
    else if (std::string("reflective") == material["type"].GetString()) type = MaterialType::Reflective;
    else if (std::string("refractive") == material["type"].GetString()) type = MaterialType::Refractive;
    MaterialParameters mat_params;
    if (material.HasMember("albedo")) mat_params.albedo = get_vec3(material["albedo"]);
    if (material.HasMember("albedo_texture_index")) mat_params.albedo_texture_idx = material["albedo_texture_index"].GetInt();
    if (material.HasMember("ior")) mat_params.ior = material["ior"].GetFloat();
    if (material.HasMember("smooth_shading")) mat_params.smooth_shading = material["smooth_shading"].GetBool();
    scene_builder.get_geometry().add_material(Material(type, mat_params));
  }
}

Object load_object(const auto& rj_object)
{
  const auto& rj_vertices = rj_object["vertices"].GetArray();
  std::vector<Vertex> vertices;
  for (size_t i = 0; i < rj_vertices.Size(); i += 3)
  {
    Vertex vertex;
    vertex.pos = get_vec3(rj_vertices, i);
    if (rj_object.HasMember("uvs"))
    {
      glm::vec3 uvs = get_vec3(rj_object["uvs"].GetArray(), i);
      vertex.tex_coords = glm::vec2(uvs.x, uvs.y);
    }
    vertices.emplace_back(vertex);
  }

  const auto& rj_indices = rj_object["triangles"].GetArray();
  std::vector<uint32_t> indices;
  for (size_t i = 0; i < rj_indices.Size(); i++)
  {
    indices.emplace_back(rj_indices[i].GetInt());
  }
  uint32_t material_idx = rj_object["material_index"].GetUint();
  return Object(vertices, indices, SpatialConfiguration(), material_idx, true);
}

void load_objects(const auto& rj_objects, SceneBuilder& scene_builder)
{
  for (const auto& object : rj_objects)
  {
    scene_builder.get_geometry().add_object(load_object(object));
  }
}

int load_scene_file(const std::string& file_path, SceneFile& scene_file)
{
  const std::string path(std::string("assets/scenes/") + file_path);
  SceneBuilder scene_builder;
  rapidjson::Document doc;
  if (load_file(path, doc) != 0) return 1;

  if (doc.HasMember("settings"))
  {
    const auto& rj_settings = doc["settings"];
    if (rj_settings.HasMember("background_color")) scene_builder.set_background(Color(get_vec3(rj_settings["background_color"].GetArray())));
    if (rj_settings.HasMember("resolution"))
    {
      scene_file.settings.resolution.x = rj_settings["resolution"]["width"].GetUint();
      scene_file.settings.resolution.y = rj_settings["resolution"]["height"].GetUint();
    }
    if (rj_settings.HasMember("bucket_size")) scene_file.settings.bucket_size = rj_settings["bucket_size"].GetUint();
    if (rj_settings.HasMember("max_path_length")) scene_file.settings.max_path_length = rj_settings["max_path_length"].GetUint();
  }

  const auto& rj_cam = doc["camera"];
  scene_builder.get_camera().get_spatial_conf().set_position(get_vec3(rj_cam["position"]));
  if (rj_cam.HasMember("focal_length")) scene_builder.get_camera().set_focal_length(rj_cam["focal_length"].GetFloat());
  if (rj_cam.HasMember("sensor_size")) scene_builder.get_camera().set_sensor_size(rj_cam["sensor_size"].GetFloat());

  if (doc.HasMember("lights"))
  {
    load_lights(doc["lights"].GetArray(), scene_builder);
  }

  if (doc.HasMember("textures"))
  {
    load_textures(doc["textures"].GetArray(), scene_builder);
  }

  if (doc.HasMember("materials"))
  {
    load_materials(doc["materials"].GetArray(), scene_builder);
  }

  load_objects(doc["objects"].GetArray(), scene_builder);

  scene_file.scene = std::make_shared<Scene>(scene_builder.build_scene());
  return 0;
}
