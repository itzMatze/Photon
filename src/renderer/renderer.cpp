#include "renderer/renderer.hpp"

#include <iostream>
#include <memory>
#include <thread>

#include "renderer/output.hpp"
#include "renderer/rendering_algorithms.hpp"
#include "util/timer.hpp"
#include "image/image_file_handler.hpp"
#include "renderer/color.hpp"
#include "scene/scene_file_handler.hpp"
#include "util/random_generator.hpp"
#include "util/vec2.hpp"
#include "util/log.hpp"
#include "util/timer.hpp"

enum class SignalFlags : uint32_t
{
  None = 0,
  Stop = (1 << 0),
  PreventOutputAccess = (1 << 1),
};

using Signals = NamedBitfield<SignalFlags>;
ENABLE_ENUM_OPERATORS(SignalFlags);

void Renderer::init(SceneFile& scene_file, const std::string& name, const Settings& settings)
{
  Timer t;
  phlog::debug("Renderer initializing");
  phlog::debug("Renderer using {} threads", settings.thread_count);
  this->settings = settings;
  this->scene_file = scene_file;
  output_name = name;
  output = std::make_shared<Output>(scene_file.settings.resolution, OutputTargetFlags::ColorArray | OutputTargetFlags::SDLSurface);
  if (settings.show_preview_window) preview_window = std::make_unique<Window>(1800, scene_file.settings.resolution);
  buckets.clear();

  // divide image into buckets that can be rendered concurrently
  const glm::uvec2 bucket_size = glm::uvec2(scene_file.settings.bucket_size, scene_file.settings.bucket_size);
  const glm::uvec2 bucket_count = (scene_file.settings.resolution / bucket_size);
  // add one overflow bucket if the buckets do not fit the resolution
  const glm::uvec2 overflow_bucket_size = glm::uvec2(scene_file.settings.resolution.x % bucket_size.x, scene_file.settings.resolution.y % bucket_size.y);
  for (uint32_t x = 0; x < bucket_count.x; x++)
  {
    for (uint32_t y = 0; y < bucket_count.y; y++)
    {
      buckets.emplace_back(bucket_size * glm::uvec2(x, y), bucket_size * glm::uvec2(x + 1, y + 1));
    }
    if (overflow_bucket_size.y > 0) buckets.emplace_back(bucket_size * glm::uvec2(x, bucket_count.y), bucket_size * glm::uvec2(x + 1, bucket_count.y) + glm::uvec2(0, overflow_bucket_size.y));
  }
  if (overflow_bucket_size.x > 0)
  {
    for (uint32_t y = 0; y < bucket_count.y; y++)
    {
      buckets.emplace_back(bucket_size * glm::uvec2(bucket_count.x, y), bucket_size * glm::uvec2(bucket_count.x, y + 1) + glm::uvec2(overflow_bucket_size.x, 0));
    }
  }
  phlog::info("Successfully initialized renderer in {}ms", t.elapsed<std::milli>());
}

void Renderer::render()
{
  Timer t;
  phlog::debug("Starting rendering");
  assert(scene_file.scene);
  assert(output);
  if (!scene_file.scene->is_animated())
  {
    phlog::debug("Scene is not animated");
    if (!render_frame()) return;
    phlog::info("Successfully finished rendering in {}ms", t.restart<std::milli>());
    phlog::debug("Saving image");
    save_single_image(Bitmap(output->get_pixels(), scene_file.settings.resolution), output_name, FileType::png);
    phlog::debug("Successfully saved image in {}ms", t.restart<std::milli>());
  }
  else
  {
    phlog::debug("Scene is animated");
    ImageSeries image_series(output_name, FileType::png);
    uint32_t frame_idx = 0;
    while (scene_file.scene->step())
    {
      Timer t;
      phlog::debug("Rendering frame {}", frame_idx);
      if (!render_frame()) return;
      phlog::debug("Successfully rendered frame {} in {}ms", frame_idx, t.restart<std::milli>());
      phlog::debug("Saving frame {} to image", frame_idx);
      image_series.save_image(Bitmap(output->get_pixels(), scene_file.settings.resolution), frame_idx);
      phlog::debug("Successfully saved frame {} to image in {}ms", frame_idx, t.restart<std::milli>());
      frame_idx++;
    }
    phlog::info("Successfully finished rendering in {}ms", t.restart<std::milli>());
  }
}

glm::vec2 get_camera_coordinates(glm::uvec2 resolution, glm::uvec2 pixel, bool use_jittering)
{
  // offset to either get a random position inside of the pixel square or the center of the pixel
  glm::vec2 offset = use_jittering ? glm::vec2(rng::random_float(), rng::random_float()) : glm::vec2(0.5);
  glm::vec2 pixel_coordinates = (glm::vec2(pixel) + offset) / glm::vec2(resolution);
  float aspect_ratio = float(resolution.y) / float(resolution.x);
  pixel_coordinates.y *= aspect_ratio;
  pixel_coordinates.y += (1.0 - aspect_ratio) / 2.0;
  return pixel_coordinates;
}

void render_buckets(const SceneFile* scene_file,
                    const Renderer::Settings* renderer_settings,
                    std::shared_ptr<Output> output,
                    std::atomic<uint32_t>* bucket_idx,
                    const std::vector<ImageBucket>* buckets,
                    Signals* signals_receiver,
                    Signals* signals_sender,
                    uint32_t thread_idx)
{
  // atomically get next bucket index in each iteration and check whether the index is still valid
  for (uint32_t local_bucket_idx = bucket_idx->fetch_add(1); local_bucket_idx < buckets->size(); local_bucket_idx = bucket_idx->fetch_add(1))
  {
    const ImageBucket& bucket = (*buckets)[local_bucket_idx];
    for (uint32_t y = bucket.min.y; y < bucket.max.y; y++)
    {
      for (uint32_t x = bucket.min.x; x < bucket.max.x; x++)
      {
        if ((*signals_receiver) & SignalFlags::Stop) return;
        Color color = whitted_ray_trace(*scene_file, get_camera_coordinates(scene_file->settings.resolution, {x, y}, renderer_settings->use_jittering));
        // wait if preview is currently updated
        while ((*signals_receiver) & SignalFlags::PreventOutputAccess);
        (*signals_sender) |= SignalFlags::PreventOutputAccess;
        output->set_pixel(x, y, color);
        (*signals_sender) &= ~SignalFlags::PreventOutputAccess;
      }
    }
  }
}

bool Renderer::render_frame()
{
  std::atomic<uint32_t> bucket_idx = 0;
  // prevent false sharing by padding signals to 64 bytes
  struct PaddedSignals {
    Signals signals;
    uint8_t pad[64 - sizeof(Signals)];
  };
  // create enum bitfields to send signals from master thread to slaves and communicate back
  std::vector<PaddedSignals> thread_signals(settings.thread_count + 1);
  std::vector<std::jthread> threads;
  for (uint32_t i = 0; i < settings.thread_count; i++) threads.push_back(std::jthread(&render_buckets, &scene_file, &settings, output, &bucket_idx, &buckets, &(thread_signals.back().signals), &(thread_signals[i].signals), i));
  // if the preview window should not be shown we can return immediately
  if (!preview_window)
  {
    threads.clear();
    return true;
  }
  // otherwise update the window at fixed time steps and after rendering is finished
  Timer t;
  // each thread will increase the index once when querying it after the last bucket has been taken
  while (bucket_idx.load() < buckets.size() + settings.thread_count)
  {
    // window received exit command, stop rendering and return early
    if (!preview_window->get_inputs())
    {
      phlog::info("Received exit signal, aborting rendering");
      thread_signals.back().signals |= SignalFlags::Stop;
      threads.clear();
      return false;
    }
    if (t.elapsed() > 0.5)
    {
      t.restart();
      thread_signals.back().signals |= SignalFlags::PreventOutputAccess;
      for (uint32_t i = 0; i < settings.thread_count; i++)
      {
        // wait for all threads to finish writing their pixel
        while (thread_signals[i].signals & SignalFlags::PreventOutputAccess);
      }
      preview_window->update_content(output->get_sdl_surface());
      thread_signals.back().signals &= ~SignalFlags::PreventOutputAccess;
    }
    std::this_thread::yield();
  }
  preview_window->update_content(output->get_sdl_surface());
  threads.clear();
  return true;
}
