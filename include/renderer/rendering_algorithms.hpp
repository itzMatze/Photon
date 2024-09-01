#pragma once

#include "renderer/output.hpp"
#include "renderer/thread_signals.hpp"
#include "scene/scene_file_handler.hpp"

enum class RenderingAlgorithms
{
  WhittedRayTracing,
  PathTracing
};

struct WhittedSettings
{
};

void whitted_ray_trace(const SceneFile& scene_file,
                       const WhittedSettings& whitted_settings,
                       std::shared_ptr<Output> output,
                       Signals* master_signals,
                       std::vector<Signals*>* thread_signals,
                       uint32_t thread_count);

struct PathTracingSettings
{
  uint32_t sample_count;
  bool use_jittering;
};

void path_trace(const SceneFile& scene_file,
                const PathTracingSettings& settings,
                std::shared_ptr<Output> output,
                Signals* master_signals,
                std::vector<Signals*>* thread_signals,
                uint32_t thread_count);
