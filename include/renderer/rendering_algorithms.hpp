#pragma once

#include "renderer/output.hpp"
#include "renderer/thread_signals.hpp"
#include "scene/scene_file_handler.hpp"

enum class RenderingAlgorithms
{
  WhittedRayTracing,
  PathTracing
};

inline std::vector<std::string> get_algorithm_name_list()
{
  return std::vector<std::string>{"whitted", "path_tracing"};
}

inline int get_algorithm(const std::string& algorithm_name, RenderingAlgorithms& algorithm)
{
  if (algorithm_name == "whitted") algorithm = RenderingAlgorithms::WhittedRayTracing;
  else if (algorithm_name == "path_tracing") algorithm = RenderingAlgorithms::PathTracing;
  else return 1;
  return 0;
}

struct WhittedSettings
{
};

void whitted_ray_trace(const WhittedSettings& whitted_settings,
                       const SceneFile& scene_file,
                       std::shared_ptr<Output> output,
                       Signals* master_signals,
                       std::vector<Signals*>* thread_signals,
                       uint32_t thread_count);

struct PathTracingSettings
{
  uint32_t sample_count;
  uint32_t nee_sample_count;
  bool use_jittering;
};

void path_trace(const PathTracingSettings& settings,
                const SceneFile& scene_file,
                std::shared_ptr<Output> output,
                Signals* master_signals,
                std::vector<Signals*>* thread_signals,
                uint32_t thread_count);
