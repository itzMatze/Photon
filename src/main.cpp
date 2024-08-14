#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "renderer/color.hpp"
#include "renderer/renderer.hpp"
#include "scene/scene_factory.hpp"
#include "scene/scene_file_handler.hpp"
#include "util/timer.hpp"
#include "util/vec2.hpp"
#include "util/log.hpp"

static constexpr glm::uvec2 resolution(1920, 1080);

int main(int argc, char** argv)
{
  std::vector<spdlog::sink_ptr> sinks;
  sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_st>());
  sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_st>("photon.log", true));
  auto combined_logger = std::make_shared<spdlog::logger>("default_logger", sinks.begin(), sinks.end());
  spdlog::set_default_logger(combined_logger);
  spdlog::set_level(spdlog::level::debug);
  spdlog::set_pattern("[%Y-%m-%d %T.%e] [%L] %v");

  std::vector<Color> pixels;
  SceneFile scene_file;
  Renderer renderer;
  const uint32_t thread_count = std::thread::hardware_concurrency();
  Timer t;
  std::string scene_filename("scene0.phene");
  if (load_scene_file(scene_filename, scene_file) != 0) return 1;
  renderer.init(scene_file, "image", {.thread_count = thread_count, .show_preview_window = true});
  renderer.render();
  return 0;
}

