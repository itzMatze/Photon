#include "renderer/renderer.hpp"
#include "util/timer.hpp"
#include <atomic>
#include <iostream>
#include <thread>
#include "image/image_file_handler.hpp"
#include "renderer/color.hpp"
#include "scene/scene_file_handler.hpp"
#include "util/random_generator.hpp"
#include "util/vec2.hpp"

void Renderer::init(const SceneFile& scene_file, const std::string& name, const Settings& settings)
{
  this->settings = settings;
  scene = scene_file.scene;
  resolution = scene_file.settings.resolution;
  output_name = name;
  buckets.clear();

  // divide image into buckets that can be rendered concurrently
  const cm::Vec2u bucket_count = (resolution / scene_file.settings.bucket_size);
  const cm::Vec2u bucket_size = cm::Vec2u(scene_file.settings.bucket_size, scene_file.settings.bucket_size);
  // add one overflow bucket if the buckets do not fit the resolution
  const cm::Vec2u overflow_bucket_size = cm::Vec2u(resolution.x % scene_file.settings.bucket_size, resolution.y % scene_file.settings.bucket_size);
  for (uint32_t x = 0; x < bucket_count.x; x++)
  {
    for (uint32_t y = 0; y < bucket_count.y; y++)
    {
      buckets.emplace_back(bucket_size * cm::Vec2u(x, y), bucket_size * cm::Vec2u(x + 1, y + 1));
    }
    if (overflow_bucket_size.x > 0) buckets.emplace_back(bucket_size * cm::Vec2u(x, bucket_count.y), bucket_size * cm::Vec2u(x + 1, bucket_count.y + overflow_bucket_size.y));
  }
  if (overflow_bucket_size.y > 0)
  {
    for (uint32_t y = 0; y < bucket_count.y; y++)
    {
      buckets.emplace_back(bucket_size * cm::Vec2u(bucket_count.x, y), bucket_size * cm::Vec2u(bucket_count.x + overflow_bucket_size.x, y));
    }
  }
}

void Renderer::render()
{
  assert(scene);
  if (!scene->is_animated())
  {
    std::vector<Color> pixels = render_frame();
    save_single_image(pixels, output_name, resolution, FileType::png);
  }
  else
  {
    ImageSeries image_series(output_name, resolution, FileType::png);
    uint32_t frame_idx = 0;
    while (scene->step())
    {
      Timer t;
      std::vector<Color> pixels = render_frame();
      std::cout << "frametime " << frame_idx << ": " << t.restart<std::milli>() << "ms" << std::endl;
      image_series.save_image(pixels, frame_idx);
      frame_idx++;
    }
  }
}

std::vector<Color> Renderer::render_frame() const
{
  std::vector<Color> pixels(resolution.x * resolution.y);
  std::atomic<uint32_t> bucket_idx = 0;
  if (settings.thread_count < 2)
  {
    render_buckets(&pixels, &bucket_idx);
  }
  else
  {
    std::vector<std::jthread> threads;
    for (uint32_t i = 0; i < settings.thread_count; i++) threads.push_back(std::jthread(&Renderer::render_buckets, this, &pixels, &bucket_idx));
  }
  return pixels;
}

void Renderer::render_buckets(std::vector<Color>* pixels, std::atomic<uint32_t>* bucket_idx) const
{
  // store all rays that need to be traced, their accumulated attenuation, and their depth
  struct PathVertex
  {
    Ray ray;
    cm::Vec3 attenuation;
    uint32_t depth;
  };
  std::vector<PathVertex> path_vertices;
  HitInfo hit_info;
  // atomically get next bucket index in each iteration and check whether the index is still valid
  for (uint32_t local_bucket_idx = bucket_idx->fetch_add(1); local_bucket_idx < buckets.size(); local_bucket_idx = bucket_idx->fetch_add(1))
  {
    const ImageBucket bucket = buckets[local_bucket_idx];
    for (uint32_t y = bucket.min.y; y < bucket.max.y; y++)
    {
      for (uint32_t x = bucket.min.x; x < bucket.max.x; x++)
      {
        path_vertices.clear();
        // add initial vertex of the camera
        path_vertices.push_back(PathVertex{scene->get_camera().get_ray(get_camera_coordinates({x, y})), cm::Vec3(1.0, 1.0, 1.0), 0});
        Color color(0.0, 0.0, 0.0);
        // trace rays as long as there are rays left to trace
        while (!path_vertices.empty())
        {
          PathVertex path_vertex = path_vertices.back();
          path_vertices.pop_back();
          if (scene->get_geometry().intersect(path_vertex.ray, hit_info))
          {
            Material material = (hit_info.material_idx == -1) ? Material() : scene->get_geometry().get_materials()[hit_info.material_idx];
            std::vector<BSDFSample> bsdf_samples = material.get_bsdf_samples(hit_info, path_vertex.ray.get_dir());
            for (const auto& bsdf_sample : bsdf_samples)
            {
              const uint32_t depth = path_vertex.depth + 1;
              if (depth < settings.max_path_length)
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
                for (const auto& light : scene->get_lights())
                {
                  const cm::Vec3 outgoing_dir = cm::normalize(light.get_position() - hit_info.pos);
                  const float light_distance = cm::length(light.get_position() - hit_info.pos);
                  // trace shadow ray with small offset in the direction of the normal to avoid shadow acne
                  const Ray shadow_ray(hit_info.pos + RAY_START_OFFSET * hit_info.geometric_normal, outgoing_dir, RayConfig{.max_t = light_distance, .anyhit = true, .backface_culling = false});
                  HitInfo shadow_hit_info;
                  if (scene->get_geometry().intersect(shadow_ray, shadow_hit_info)) continue;
                  const float light_surface = 4.0 * M_PI * light_distance * light_distance;
                  cm::Vec3 contribution = cm::Vec3(light.get_intensity() / light_surface);
                  contribution *= path_vertex.attenuation * material.eval(hit_info, path_vertex.ray.get_dir(), outgoing_dir);
                  color.value += contribution;
                }
              }
            }
          }
          else
          {
            // background color
            color.value += scene->get_background_color().value * path_vertex.attenuation;
          }
        }
        // does not require synchronization because every thread writes different pixels
        (*pixels)[y * resolution.x + x] = color;
      }
    }
  }
}

cm::Vec2 Renderer::get_camera_coordinates(cm::Vec2u pixel) const
{
  // offset to either get a random position inside of the pixel square or the center of the pixel
  cm::Vec2 offset = settings.use_jittering ? cm::Vec2(rng::random_float(), rng::random_float()) : cm::Vec2(0.5);
  cm::Vec2 pixel_coordinates = (cm::Vec2(pixel) + offset) / cm::Vec2(resolution);
  float aspect_ratio = float(resolution.y) / float(resolution.x);
  pixel_coordinates.y *= aspect_ratio;
  pixel_coordinates.y += (1.0 - aspect_ratio) / 2.0;
  return pixel_coordinates;
}

