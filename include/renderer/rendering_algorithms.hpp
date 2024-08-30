#pragma once

#include "renderer/output.hpp"
#include "renderer/thread_signals.hpp"
#include "scene/scene_file_handler.hpp"

struct WhittedSettings
{
  uint32_t thread_count;
};

void whitted_ray_trace(const SceneFile& scene_file, const WhittedSettings& settings, std::shared_ptr<Output> output, Signals* master_signals, std::vector<Signals*>* thread_signals);
