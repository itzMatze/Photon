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
    cm::Vec2u min;
    cm::Vec2u max;
  };

  std::shared_ptr<Scene> scene;
  cm::Vec2u resolution;
  std::string output_name;
  std::vector<ImageBucket> buckets;
  Settings settings;

  std::vector<Color> render_frame() const;
  void render_buckets(std::vector<Color>* pixels, std::atomic<uint32_t>* bucket_idx) const;
  cm::Vec2 get_camera_coordinates(cm::Vec2u pixel) const;
};

