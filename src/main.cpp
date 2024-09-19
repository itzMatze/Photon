#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "argparse/argparse.hpp"

#include "renderer/color.hpp"
#include "renderer/renderer.hpp"
#include "renderer/rendering_algorithms.hpp"
#include "scene/scene_factory.hpp"
#include "scene/scene_file_handler.hpp"
#include "util/timer.hpp"
#include "util/vec2.hpp"
#include "util/log.hpp"

static constexpr glm::uvec2 resolution(1920, 1080);

int parse_args(int argc, char** argv, argparse::ArgumentParser& program) {
  program.add_argument("scene_file").required().help("Specify .phene file that is loaded and rendered");
  program.add_argument("--show_preview").flag().help("Show preview window");
  program.add_argument("--thread_count").scan<'u', uint32_t>().help("Number of threads to use");
  argparse::Argument& algorithm_arg = program.add_argument("--algorithm").help("Rendering algorithm to use");
  std::vector<std::string> algorithm_choices = get_algorithm_name_list();
  for (const auto choice : algorithm_choices) algorithm_arg.add_choice(choice);
  program.add_argument("--sample_count").scan<'u', uint32_t>().help("Number of samples per pixel");
  program.add_argument("--nee_sample_count").scan<'u', uint32_t>().help("Number of nee samples at every path vertex");

  try {
    program.parse_args(argc, argv);
    if (program.get<bool>("--help")) {
      std::cout << program << std::endl;
      return 1;
    }
  } catch (const std::exception& err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    return 1;
  }
  return 0;
}

Renderer::Settings get_settings(const argparse::ArgumentParser& program)
{
  uint32_t sample_count = 128;
  uint32_t nee_sample_count = 2;
  RenderingAlgorithms rendering_algorithm = RenderingAlgorithms::PathTracing;
  const uint32_t thread_count = program.is_used("--thread_count") ? program.get<uint32_t>("--thread_count") : std::thread::hardware_concurrency();
  const bool show_preview = program.get<bool>("--show_preview");
  if (program.is_used("--algorithm")) get_algorithm(program.get<std::string>("--algorithm"), rendering_algorithm);
  if (program.is_used("--sample_count")) sample_count = program.get<uint32_t>("--sample_count");
  if (program.is_used("--nee_sample_count")) nee_sample_count = program.get<uint32_t>("--nee_sample_count");
  if (rendering_algorithm == RenderingAlgorithms::WhittedRayTracing) return Renderer::Settings(WhittedSettings{}, thread_count, show_preview);
  else if (rendering_algorithm == RenderingAlgorithms::PathTracing) return Renderer::Settings(PathTracingSettings{.sample_count = sample_count, .nee_sample_count = nee_sample_count, .use_jittering = true}, thread_count, show_preview);
  else PH_THROW("Unknown rendering algorithm!");
}

int main(int argc, char** argv)
{
  argparse::ArgumentParser program("Photon");
  int parse_status = parse_args(argc, argv, program);
  if (parse_status != 0) return parse_status;

  std::vector<spdlog::sink_ptr> sinks;
  sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_st>());
  sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_st>("photon.log", true));
  auto combined_logger = std::make_shared<spdlog::logger>("default_logger", sinks.begin(), sinks.end());
  spdlog::set_default_logger(combined_logger);
  spdlog::set_level(spdlog::level::debug);
  spdlog::set_pattern("[%Y-%m-%d %T.%e] [%L] %v");

  SceneFile scene_file;
  Renderer renderer;
  Timer t;
  std::string scene_filename(program.get<std::string>("scene_file"));
  if (load_scene_file(scene_filename, scene_file) != 0) return 1;
  Renderer::Settings renderer_settings = get_settings(program);
  renderer.render(scene_file, "image", renderer_settings);
  return 0;
}

