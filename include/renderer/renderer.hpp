#pragma once
#include <string>
#include "renderer/output.hpp"
#include "renderer/rendering_algorithms.hpp"
#include "renderer/window.hpp"
#include "scene/scene_file_handler.hpp"
#include "util/vec2.hpp"

class Renderer
{
public:
  struct Settings
  {
  public:
    Settings(const PathTracingSettings& path_tracing_settings, uint32_t thread_count, bool show_preview_window)
      : path_tracing_settings(path_tracing_settings), rendering_algorithm(RenderingAlgorithms::PathTracing), thread_count(thread_count), show_preview_window(show_preview_window) {}

    Settings(const WhittedSettings& whitted_settings, uint32_t thread_count, bool show_preview_window)
      : whitted_settings(whitted_settings), rendering_algorithm(RenderingAlgorithms::WhittedRayTracing), thread_count(thread_count), show_preview_window(show_preview_window) {}

    union
    {
      const WhittedSettings whitted_settings;
      const PathTracingSettings path_tracing_settings;
    };
    const RenderingAlgorithms rendering_algorithm;
    const uint32_t thread_count;
    const bool show_preview_window;
  };
  Renderer() = default;
  void render(SceneFile& scene_file, const std::string& name, const Settings& settings);

private:
  std::shared_ptr<Output> output;
  std::unique_ptr<Window> preview_window;

  void init(SceneFile& scene_file, const std::string& name, const Settings& settings);
  bool render_frame(SceneFile& scene_file, const Settings& settings);
};

