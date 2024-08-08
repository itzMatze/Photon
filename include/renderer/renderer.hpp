#pragma once
#include <string>
#include "renderer/color.hpp"
#include "scene/scene.hpp"
#include "scene/scene_file_handler.hpp"
#include "util/vec2.hpp"

class Renderer
{
public:
  struct Settings
  {
    uint32_t thread_count = 1;
    uint32_t max_path_length = 16;
    bool use_jittering = false;
  };
  Renderer() = default;
  void init(const SceneFile& scene_file, const std::string& name, const Settings& settings);
  void render();

private:
  struct ImageBucket
  {
    glm::uvec2 min;
    glm::uvec2 max;
  };

  std::shared_ptr<Scene> scene;
  glm::uvec2 resolution;
  std::string output_name;
  std::vector<ImageBucket> buckets;
  Settings settings;

  std::vector<Color> render_frame() const;
  void render_buckets(std::vector<Color>* pixels, std::atomic<uint32_t>* bucket_idx) const;
  glm::vec2 get_camera_coordinates(glm::uvec2 pixel) const;
};

