#pragma once
#include "scene/scene.hpp"
#include "util/vec2.hpp"

struct Settings
{
  glm::uvec2 resolution = glm::uvec2(1920, 1080);
  uint32_t bucket_size = 20;
  uint32_t max_path_length = 16;
};

struct SceneFile
{
  std::shared_ptr<Scene> scene;
  Settings settings;
};

int load_scene_file(const std::string& file_path, SceneFile& scene_file);
int load_object_file(const std::string& file_path, Object& object);

