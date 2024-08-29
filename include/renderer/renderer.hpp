#pragma once
#include <string>
#include "renderer/output.hpp"
#include "renderer/window.hpp"
#include "scene/scene_file_handler.hpp"
#include "util/vec2.hpp"

struct ImageBucket
{
  glm::uvec2 min;
  glm::uvec2 max;
};

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
  void render(SceneFile& scene_file, const std::string& name, const Settings& settings);

private:

  std::vector<ImageBucket> buckets;
  std::shared_ptr<Output> output;
  std::unique_ptr<Window> preview_window;

  void init(SceneFile& scene_file, const std::string& name, const Settings& settings);
  bool render_frame(SceneFile& scene_file, const Settings& settings);
};

