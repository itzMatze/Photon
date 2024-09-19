#include "renderer/renderer.hpp"

#include <memory>
#include <thread>

#include "renderer/output.hpp"
#include "renderer/rendering_algorithms.hpp"
#include "util/timer.hpp"
#include "image/image_file_handler.hpp"
#include "renderer/thread_signals.hpp"
#include "scene/scene_file_handler.hpp"
#include "util/vec2.hpp"
#include "util/log.hpp"
#include "util/timer.hpp"

void Renderer::init(SceneFile& scene_file, const std::string& name, const Settings& settings)
{
  Timer t;
  phlog::debug("Renderer initializing");
  phlog::debug("Renderer using {} threads", settings.thread_count);
  output = std::make_shared<Output>(scene_file.settings.resolution, OutputTargetFlags::ColorArray | OutputTargetFlags::SDLSurface);
  if (settings.show_preview_window) preview_window = std::make_unique<Window>(1800, scene_file.settings.resolution);
  phlog::info("Successfully initialized renderer in {}ms", t.elapsed<std::milli>());
}

void Renderer::render(SceneFile& scene_file, const std::string& name, const Settings& settings)
{
  init(scene_file, name, settings);
  Timer t;
  phlog::debug("Starting rendering");
  assert(scene_file.scene);
  assert(output);
  if (!scene_file.scene->is_animated())
  {
    phlog::debug("Scene is not animated");
    if (!render_frame(scene_file, settings)) return;
    phlog::info("Successfully finished rendering in {}ms", t.restart<std::milli>());
    phlog::debug("Saving image");
    save_single_image(Bitmap(output->get_pixels(), scene_file.settings.resolution), name, FileType::png);
    phlog::debug("Successfully saved image in {}ms", t.restart<std::milli>());
  }
  else
  {
    phlog::debug("Scene is animated");
    ImageSeries image_series(name, FileType::png);
    uint32_t frame_idx = 0;
    while (scene_file.scene->step())
    {
      Timer t;
      phlog::debug("Rendering frame {}", frame_idx);
      if (!render_frame(scene_file, settings)) return;
      phlog::debug("Successfully rendered frame {} in {}ms", frame_idx, t.restart<std::milli>());
      phlog::debug("Saving frame {} to image", frame_idx);
      image_series.save_image(Bitmap(output->get_pixels(), scene_file.settings.resolution), frame_idx);
      phlog::debug("Successfully saved frame {} to image in {}ms", frame_idx, t.restart<std::milli>());
      frame_idx++;
    }
    phlog::info("Successfully finished rendering in {}ms", t.restart<std::milli>());
  }
}

bool Renderer::render_frame(SceneFile& scene_file, const Settings& settings)
{
  // prevent false sharing by padding signals to 64 bytes
  struct PaddedSignals {
    Signals signals;
    uint8_t pad[64 - sizeof(Signals)];
  };
  // create enum bitfields to send signals from master thread to slaves and communicate back
  std::vector<PaddedSignals> thread_signals_storage(settings.thread_count);
  std::vector<Signals*> thread_signals(settings.thread_count);
  for (uint32_t i = 0; i < thread_signals_storage.size(); i++) thread_signals[i] = &(thread_signals_storage[i].signals);
  Signals master_signals;
  std::jthread rendering_main_thread;
  switch (settings.rendering_algorithm)
  {
    case RenderingAlgorithms::WhittedRayTracing:
    {
      rendering_main_thread = std::jthread(&whitted_ray_trace, settings.whitted_settings, scene_file, output, &master_signals, &thread_signals, settings.thread_count);
      break;
    }
    case RenderingAlgorithms::PathTracing:
    {
      rendering_main_thread = std::jthread(&path_trace, settings.path_tracing_settings, scene_file, output, &master_signals, &thread_signals, settings.thread_count);
      break;
    }
  }
  // exit only after rendering is finished
  auto check_threads_done = [&]() -> bool {
    bool done = true;
    for (uint32_t i = 0; i < settings.thread_count; i++)
    {
      const Signals& signal = *(thread_signals[i]);
      done &= (signal & SignalFlags::Done);
    }
    return done;
  };
  // if the preview window should not be shown there is nothing to do
  if (!preview_window)
  {
    while (!check_threads_done()) std::this_thread::yield();
    return true;
  }
  // otherwise update the window at fixed time steps
  Timer t;
  while (!check_threads_done())
  {
    // window received exit command, stop rendering and return early
    if (!preview_window->get_inputs())
    {
      phlog::info("Received exit signal, aborting rendering");
      master_signals |= SignalFlags::Stop;
      return false;
    }
    if (t.elapsed() > 0.5)
    {
      t.restart();
      master_signals |= SignalFlags::PreventOutputAccess;
      for (uint32_t i = 0; i < settings.thread_count; i++)
      {
        // wait for all threads to finish writing their pixel
        while (*(thread_signals[i]) & SignalFlags::PreventOutputAccess);
      }
      preview_window->update_content(output->get_sdl_surface());
      master_signals &= ~SignalFlags::PreventOutputAccess;
    }
    std::this_thread::yield();
  }
  preview_window->update_content(output->get_sdl_surface());
  return true;
}
