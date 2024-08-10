#pragma once
#include <string>
#include "renderer/output.hpp"
#include "renderer/window.hpp"
#include "scene/scene_file_handler.hpp"
#include "util/vec2.hpp"

class Renderer
{
public:
  struct Settings
  {
    uint32_t thread_count = 1;
    bool use_jittering = false;
    bool show_preview_window = true;
  };
  Renderer() = default;
  void init(SceneFile& scene_file, const std::string& name, const Settings& settings);
  void render();

private:
  struct ImageBucket
  {
    glm::uvec2 min;
    glm::uvec2 max;
  };

  SceneFile scene_file;
  std::string output_name;
  std::vector<ImageBucket> buckets;
  Settings settings;
  std::shared_ptr<Output> output;
  std::unique_ptr<Window> preview_window;

  glm::vec2 get_camera_coordinates(glm::uvec2 pixel) const;
  void render_buckets(std::atomic<uint32_t>* bucket_idx) const;
  bool render_frame();
};

