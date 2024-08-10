#include "renderer/renderer.hpp"
#include "renderer/output.hpp"
#include "util/timer.hpp"
#include <atomic>
#include <iostream>
#include <thread>
#include "image/image_file_handler.hpp"
#include "renderer/color.hpp"
#include "scene/scene_file_handler.hpp"
#include "util/random_generator.hpp"
#include "util/vec2.hpp"

void Renderer::init(SceneFile& scene_file, const std::string& name, const Settings& settings)
{
  this->settings = settings;
  this->scene_file = scene_file;
  output_name = name;
  output = std::make_shared<Output>(scene_file.settings.resolution, OutputTargetFlags::ColorArray | OutputTargetFlags::SDLSurface);
  if (settings.show_preview_window) preview_window = std::make_unique<Window>(1800, scene_file.settings.resolution);
  buckets.clear();

  // divide image into buckets that can be rendered concurrently
  const glm::uvec2 bucket_count = (scene_file.settings.resolution / scene_file.settings.bucket_size);
  const glm::uvec2 bucket_size = glm::uvec2(scene_file.settings.bucket_size, scene_file.settings.bucket_size);
  // add one overflow bucket if the buckets do not fit the resolution
  const glm::uvec2 overflow_bucket_size = glm::uvec2(scene_file.settings.resolution.x % scene_file.settings.bucket_size, scene_file.settings.resolution.y % scene_file.settings.bucket_size);
  for (uint32_t x = 0; x < bucket_count.x; x++)
  {
    for (uint32_t y = 0; y < bucket_count.y; y++)
    {
      buckets.emplace_back(bucket_size * glm::uvec2(x, y), bucket_size * glm::uvec2(x + 1, y + 1));
    }
    if (overflow_bucket_size.x > 0) buckets.emplace_back(bucket_size * glm::uvec2(x, bucket_count.y), bucket_size * glm::uvec2(x + 1, bucket_count.y + overflow_bucket_size.y));
  }
  if (overflow_bucket_size.y > 0)
  {
    for (uint32_t y = 0; y < bucket_count.y; y++)
    {
      buckets.emplace_back(bucket_size * glm::uvec2(bucket_count.x, y), bucket_size * glm::uvec2(bucket_count.x + overflow_bucket_size.x, y));
    }
  }
}

void Renderer::render()
{
  assert(scene_file.scene);
  assert(output);
  if (!scene_file.scene->is_animated())
  {
    if (!render_frame()) return;
    save_single_image(output->get_pixels(), output_name, scene_file.settings.resolution, FileType::png);
  }
  else
  {
    ImageSeries image_series(output_name, scene_file.settings.resolution, FileType::png);
    uint32_t frame_idx = 0;
    while (scene_file.scene->step())
    {
      Timer t;
      if (!render_frame()) return;
      std::cout << "frametime " << frame_idx << ": " << t.restart<std::milli>() << "ms" << std::endl;
      image_series.save_image(output->get_pixels(), frame_idx);
      frame_idx++;
    }
  }
}

glm::vec2 Renderer::get_camera_coordinates(glm::uvec2 pixel) const
{
  // offset to either get a random position inside of the pixel square or the center of the pixel
  glm::vec2 offset = settings.use_jittering ? glm::vec2(rng::random_float(), rng::random_float()) : glm::vec2(0.5);
  glm::vec2 pixel_coordinates = (glm::vec2(pixel) + offset) / glm::vec2(scene_file.settings.resolution);
  float aspect_ratio = float(scene_file.settings.resolution.y) / float(scene_file.settings.resolution.x);
  pixel_coordinates.y *= aspect_ratio;
  pixel_coordinates.y += (1.0 - aspect_ratio) / 2.0;
  return pixel_coordinates;
}

void Renderer::render_buckets(std::atomic<uint32_t>* bucket_idx) const
{
  // atomically get next bucket index in each iteration and check whether the index is still valid
  for (uint32_t local_bucket_idx = bucket_idx->fetch_add(1); local_bucket_idx < buckets.size(); local_bucket_idx = bucket_idx->fetch_add(1))
  {
    const ImageBucket& bucket = buckets[local_bucket_idx];
    for (uint32_t y = bucket.min.y; y < bucket.max.y; y++)
    {
      for (uint32_t x = bucket.min.x; x < bucket.max.x; x++)
      {
        glm::vec2 camera_coordinates = get_camera_coordinates({x, y});
        // store all rays that need to be traced, their accumulated attenuation, and their depth
        struct PathVertex
        {
          Ray ray;
          glm::vec3 attenuation;
          uint32_t depth;
        };
        std::vector<PathVertex> path_vertices;
        HitInfo hit_info;
        path_vertices.clear();
        // add initial vertex of the camera
        path_vertices.push_back(PathVertex{scene_file.scene->get_camera().get_ray(camera_coordinates), glm::vec3(1.0, 1.0, 1.0), 0});
        Color color(0.0, 0.0, 0.0);
        // trace rays as long as there are rays left to trace
        while (!path_vertices.empty())
        {
          PathVertex path_vertex = path_vertices.back();
          path_vertices.pop_back();
          if (scene_file.scene->get_geometry().intersect(path_vertex.ray, hit_info))
          {
            Material material = (hit_info.material_idx == -1) ? Material() : scene_file.scene->get_geometry().get_materials()[hit_info.material_idx];
            std::vector<BSDFSample> bsdf_samples = material.get_bsdf_samples(hit_info, path_vertex.ray.get_dir());
            for (const auto& bsdf_sample : bsdf_samples)
            {
              const uint32_t depth = path_vertex.depth + 1;
              if (depth < scene_file.settings.max_path_length)
              {
                const PathVertex next_path_vertex = PathVertex{bsdf_sample.ray, path_vertex.attenuation * bsdf_sample.attenuation, depth};
                path_vertices.push_back(next_path_vertex);
              }
            }
            // if material is dirac delta reflective or there are no lights there is no need to evaluate lighting
            if (!material.is_delta())
            {
              // if material does not depend on light (usually debug vis) just fetch albedo
              if (!material.is_light_dependent())
              {
                color.value += material.get_albedo(hit_info);
              }
              else
              {
                for (const auto& light : scene_file.scene->get_lights())
                {
                  const glm::vec3 outgoing_dir = glm::normalize(light.get_position() - hit_info.pos);
                  const float light_distance = glm::length(light.get_position() - hit_info.pos);
                  // trace shadow ray with small offset in the direction of the normal to avoid shadow acne
                  const Ray shadow_ray(hit_info.pos + RAY_START_OFFSET * hit_info.geometric_normal, outgoing_dir, RayConfig{.max_t = light_distance, .anyhit = true, .backface_culling = false});
                  HitInfo shadow_hit_info;
                  if (scene_file.scene->get_geometry().intersect(shadow_ray, shadow_hit_info)) continue;
                  const float light_surface = 4.0 * M_PI * light_distance * light_distance;
                  glm::vec3 contribution = glm::vec3(light.get_intensity() / light_surface);
                  contribution *= path_vertex.attenuation * material.eval(hit_info, path_vertex.ray.get_dir(), outgoing_dir);
                  color.value += contribution;
                }
              }
            }
          }
          else
          {
            // background color
            color.value += scene_file.scene->get_background_color().value * path_vertex.attenuation;
          }
        }
        // does not require synchronization because every thread writes different pixels
        output->set_pixel(x, y, color);
      }
    }
  }
}

bool Renderer::render_frame()
{
  std::atomic<uint32_t> bucket_idx = 0;
  std::vector<std::jthread> threads;
  for (uint32_t i = 0; i < settings.thread_count; i++) threads.push_back(std::jthread(&Renderer::render_buckets, this, &bucket_idx));
  // if the preview window should not be shown we can return immediately
  if (!preview_window) return true;
  // otherwise update the window at fixed time steps and after rendering is finished
  Timer t;
  while (bucket_idx.load() < buckets.size())
  {
    // window received exit command, stop rendering and return early
    if (!preview_window->get_inputs()) return false;
    if (t.elapsed() > 0.5)
    {
      t.restart();
      preview_window->update_content(output->get_sdl_surface());
    }
    std::this_thread::yield();
  }
  preview_window->update_content(output->get_sdl_surface());
  return true;
}
