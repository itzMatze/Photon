#include "scene/scene_file_handler.hpp"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <memory>
#include <unordered_map>

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

cm::Vec3 get_vec3(const rapidjson::GenericValue<rapidjson::UTF8<>>& rj_vec3, int base_index = 0)
{
  return cm::Vec3(rj_vec3[base_index].GetFloat(), rj_vec3[base_index + 1].GetFloat(), rj_vec3[base_index + 2].GetFloat());
}

void load_lights(const auto& rj_lights, SceneBuilder& scene_builder)
{
  for (const auto& light : rj_lights)
  {
    float intensity = light["intensity"].GetFloat();
    cm::Vec3 position = get_vec3(light["position"]);
    scene_builder.get_lights().add_new_data(Light(intensity, position));
  }
}

void load_textures(const auto& rj_textures, std::unordered_map<std::string, std::shared_ptr<Texture>>& textures, const std::string& scene_path)
{
  for (const auto& texture : rj_textures)
  {
    const std::string texture_name = texture["name"].GetString();
    if (std::string("albedo") == texture["type"].GetString())
    {
      Texture::AlbedoParameters params{get_vec3(texture["albedo"])};
      textures.emplace(texture_name, std::make_shared<Texture>(params));
    }
    else if (std::string("edges") == texture["type"].GetString())
    {
      Texture::EdgesParameters params;
      params.edge = std::make_shared<Texture>(get_vec3(texture["edge_color"]));
      params.center = std::make_shared<Texture>(get_vec3(texture["inner_color"]));
      params.thickness = texture["edge_width"].GetFloat();
      textures.emplace(texture_name, std::make_shared<Texture>(params));
    }
    else if (std::string("checker") == texture["type"].GetString())
    {
      Texture::CheckerParameters params;
      params.even = std::make_shared<Texture>(get_vec3(texture["color_A"]));
      params.odd = std::make_shared<Texture>(get_vec3(texture["color_B"]));
      params.tile_size = texture["square_size"].GetFloat();
      textures.emplace(texture_name, std::make_shared<Texture>(params));
    }
    else if (std::string("bitmap") == texture["type"].GetString())
    {
      Texture::BitmapParameters params;
      std::filesystem::path path(scene_path);
      path = path.parent_path();
      path = path.concat(texture["file_path"].GetString());
      load_image(path, params.bitmap, params.resolution);
      textures.emplace(texture_name, std::make_shared<Texture>(params));
    }
  }
}

void load_materials(const auto& rj_materials, SceneBuilder& scene_builder, std::unordered_map<std::string, std::shared_ptr<Texture>>& textures)
{
  for (const auto& material : rj_materials)
  {
    MaterialParameters mat_params;
    if (material.HasMember("albedo"))
    {
      if (material["albedo"].IsString())
      {
        const std::string texture_name = material["albedo"].GetString();
        mat_params.albedo_texture = textures.at(texture_name);
      }
      else
      {
        mat_params.albedo_texture = std::make_shared<Texture>(get_vec3(material["albedo"]));
      }
    }
    if (material.HasMember("ior")) mat_params.ior = material["ior"].GetFloat();
    if (material.HasMember("smooth_shading")) mat_params.smooth_shading = material["smooth_shading"].GetBool();
    MaterialType type;
    if (std::string("diffuse") == material["type"].GetString() || std::string("constant") == material["type"].GetString()) type = MaterialType::Diffuse;
    else if (std::string("reflective") == material["type"].GetString()) type = MaterialType::Reflective;
    else if (std::string("refractive") == material["type"].GetString()) type = MaterialType::Refractive;
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
      cm::Vec3 uvs = get_vec3(rj_object["uvs"].GetArray(), i);
      vertex.tex_coords = cm::Vec2(uvs.x, uvs.y);
    }
    vertices.emplace_back(vertex);
  }

  const auto& rj_indices = rj_object["triangles"].GetArray();
  std::vector<uint32_t> indices;
  for (size_t i = 0; i < rj_indices.Size(); i++)
  {
    indices.emplace_back(rj_indices[i].GetInt());
  }
  int32_t material_idx = -1;
  if (rj_object.HasMember("material_index")) material_idx = rj_object["material_index"].GetInt();
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
  const std::string path(std::string("../../Scenes/") + file_path);
  SceneBuilder scene_builder;
  rapidjson::Document doc;
  if (load_file(path, doc) != 0) return 1;

  const auto& rj_image_settings = doc["settings"]["image_settings"];
  scene_file.settings.resolution = cm::Vec2u(rj_image_settings["width"].GetUint(), rj_image_settings["height"].GetUint());
  // if no bucket size was set in scene file, use default value
  scene_file.settings.bucket_size = rj_image_settings.HasMember("bucket_size") ? rj_image_settings["bucket_size"].GetUint() : 20;
  const auto& rj_background_color = doc["settings"]["background_color"].GetArray();
  scene_builder.set_background(Color(rj_background_color[0].GetFloat(), rj_background_color[1].GetFloat(), rj_background_color[2].GetFloat()));

  const auto& rj_cam_matrix = doc["camera"]["matrix"];
  const cm::Mat3 orientation({get_vec3(rj_cam_matrix, 0),
                              get_vec3(rj_cam_matrix, 3),
                              get_vec3(rj_cam_matrix, 6)});
  scene_builder.get_camera().get_spatial_conf().set_orientation(orientation);

  scene_builder.get_camera().get_spatial_conf().set_position(get_vec3(doc["camera"]["position"]));

  if (doc.HasMember("lights"))
  {
    load_lights(doc["lights"].GetArray(), scene_builder);
  }

  std::unordered_map<std::string, std::shared_ptr<Texture>> textures;
  if (doc.HasMember("textures"))
  {
    load_textures(doc["textures"].GetArray(), textures, path);
  }

  if (doc.HasMember("materials"))
  {
    load_materials(doc["materials"].GetArray(), scene_builder, textures);
  }

  load_objects(doc["objects"].GetArray(), scene_builder);

  scene_builder.get_camera().set_focal_length(0.012);
  scene_file.scene = std::make_shared<Scene>(scene_builder.build_scene());
  return 0;
}

int load_object_file(const std::string& file_path, Object& object)
{
  const std::string path(std::string("../../Scenes/") + file_path);
  rapidjson::Document doc;
  if (load_file(path, doc) != 0) return 1;
  object = load_object(doc);
  return 0;
}

