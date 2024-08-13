#include "scene/scene_file_handler.hpp"

#include <iostream>
#include <fstream>
#include <memory>

#include "glm/geometric.hpp"
#include "glm/gtc/quaternion.hpp"
#include "object/model.hpp"
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
    MaterialParameters mat_params;
    if (material.HasMember("albedo")) mat_params.albedo = get_vec3(material["albedo"]);
    if (material.HasMember("albedo_texture_index")) mat_params.albedo_texture_id = material["albedo_texture_index"].GetInt();
    if (material.HasMember("emission")) mat_params.emission = get_vec3(material["emission"]);
    if (material.HasMember("emission_strength")) mat_params.emission_strength = material["emission_strength"].GetFloat();
    if (material.HasMember("roughness")) mat_params.roughness = material["roughness"].GetFloat();
    if (material.HasMember("metallic")) mat_params.metallic = material["metallic"].GetFloat();
    if (material.HasMember("transmission")) mat_params.transmission = material["transmission"].GetFloat();
    if (material.HasMember("ior")) mat_params.ior = material["ior"].GetFloat();
    if (material.HasMember("smooth_shading")) mat_params.smooth_shading = material["smooth_shading"].GetBool();
    if (material.HasMember("show_albedo")) mat_params.show_albedo = material["show_albedo"].GetBool();
    if (material.HasMember("show_bary")) mat_params.show_bary = material["show_bary"].GetBool();
    if (material.HasMember("show_normal")) mat_params.show_normal = material["show_normal"].GetBool();
    if (material.HasMember("show_tex_coords")) mat_params.show_tex_coords = material["show_tex_coords"].GetBool();
    scene_builder.get_geometry().add_material(mat_params);
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
  return Object(vertices, indices, true);
}

void load_objects(const auto& rj_objects, SceneBuilder& scene_builder)
{
  for (const auto& object : rj_objects)
  {
    if (object.IsString()) GLTFModel::load(scene_builder, object.GetString());
    else scene_builder.get_geometry().add_object(load_object(object));
  }
}

void load_instances(const auto& rj_instances, SceneBuilder& scene_builder)
{
  for (const auto& rj_instance : rj_instances)
  {
    uint32_t object_id = rj_instance["object_index"].GetUint();
    int32_t material_id = (rj_instance.HasMember("material_index")) ? rj_instance["material_index"].GetInt() : -1;
    SpatialConfiguration spatial_conf;
    if (rj_instance.HasMember("position")) spatial_conf.set_position(get_vec3(rj_instance["position"]));
    if (rj_instance.HasMember("orientation")) spatial_conf.rotate(get_vec3(rj_instance["orientation"]));
    scene_builder.get_geometry().add_object_instance(object_id, material_id, spatial_conf);
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
  if (rj_cam.HasMember("view_direction"))
  {
    const glm::vec3 view_dir = glm::normalize(get_vec3(rj_cam["view_direction"]));
    glm::vec3 up(0.0, 1.0, 0.0);
    if (rj_cam.HasMember("up")) up = glm::normalize(get_vec3(rj_cam["up"]));
    scene_builder.get_camera().get_spatial_conf().set_orientation(glm::quatLookAt(view_dir, up));
  }
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
  load_instances(doc["instances"].GetArray(), scene_builder);

  scene_file.scene = std::make_shared<Scene>(scene_builder.build_scene());
  return 0;
}
