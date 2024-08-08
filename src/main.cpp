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

static constexpr cm::Vec2u resolution(1920, 1080);

int main(int argc, char** argv)
{
  std::vector<Color> pixels;
  SceneFile scene_file;
  Renderer renderer;
  const uint32_t thread_count = std::thread::hardware_concurrency();
  std::cerr << "Using " << thread_count << " threads" << std::endl;
  Timer t;
  scene_file.scene = std::make_shared<Scene>(create_pyramid_star_scene());
  scene_file.settings.resolution = cm::Vec2u(1920, 1080);
  scene_file.settings.bucket_size = 20;
  std::cout << "Scene created: " << t.restart<std::milli>() << "ms" << std::endl;
  renderer.init(scene_file, "progression", {.thread_count = thread_count});
  std::cout << "Renderer initialized: " << t.restart<std::milli>() << "ms" << std::endl;
  renderer.render();
  std::cout << "Rendering finished: " << t.restart<std::milli>() << "ms" << std::endl;
  return 0;
}

