#include "renderer/bucket_rendering.hpp"
#include "renderer/camera.hpp"
#include "renderer/output.hpp"
#include "renderer/rendering_algorithms.hpp"
#include <thread>

Color trace(const SceneFile& scene_file, glm::vec2 camera_coordinates)
{
  // store all rays that need to be traced, their accumulated attenuation, and their depth
  struct PathVertex
  {
    Ray ray;
    glm::vec3 attenuation;
    uint32_t depth;
  };
  std::vector<PathVertex> path_vertices;
  std::vector<BSDFSample> bsdf_samples;
  HitInfo hit_info;
  Material default_material;
  // add initial vertex of the camera
  path_vertices.push_back(PathVertex{scene_file.scene->get_camera().get_ray(camera_coordinates), glm::vec3(1.0, 1.0, 1.0), 0});
  Color color(0.0, 0.0, 0.0);
  // trace rays as long as there are rays left to trace
  while (!path_vertices.empty())
  {
    const PathVertex path_vertex = path_vertices.back();
    path_vertices.pop_back();
    if (scene_file.scene->get_geometry().intersect(path_vertex.ray, hit_info))
    {
      const Material& material = (hit_info.material_id == -1) ? default_material : scene_file.scene->get_geometry().get_material(hit_info.material_id);
      color += path_vertex.attenuation * material.get_emission(hit_info);
      // whitted ray tracing can only handle perfectly transmissive, perfectly reflective, and diffuse materials
      if (material.is_delta())
      {
        const uint32_t depth = path_vertex.depth + 1;
        if (depth < scene_file.settings.max_path_length)
        {
          bsdf_samples.clear();
          material.get_bsdf_samples(hit_info, path_vertex.ray.get_dir(), bsdf_samples);
          for (const auto& bsdf_sample : bsdf_samples)
          {
            const PathVertex next_path_vertex = PathVertex{bsdf_sample.ray, path_vertex.attenuation * bsdf_sample.attenuation, depth};
            path_vertices.push_back(next_path_vertex);
          }
        }
      }
      else
      {
        if (!material.is_delta())
        {
          for (const auto& light : scene_file.scene->get_lights())
          {
            const glm::vec3 outgoing_dir = glm::normalize(light.get_position() - hit_info.pos);
            const glm::vec3 attenuation = material.eval(hit_info, path_vertex.ray.get_dir(), outgoing_dir);
            if (glm::dot(attenuation, attenuation) < 0.0000001f) continue;
            const float light_distance = glm::length(light.get_position() - hit_info.pos);
            // trace shadow ray with small offset in the direction of the normal to avoid shadow acne
            const Ray shadow_ray(hit_info.pos + RAY_START_OFFSET * hit_info.get_oriented_face_geometric_normal(), outgoing_dir, RayConfig{.max_t = light_distance, .anyhit = true, .backface_culling = false});
            HitInfo shadow_hit_info;
            if (scene_file.scene->get_geometry().intersect(shadow_ray, shadow_hit_info)) continue;
            glm::vec3 contribution = light.get_contribution(hit_info.pos);
            contribution *= path_vertex.attenuation * attenuation;
            color += contribution;
          }
        }
      }
    }
    else
    {
      // background color
      color += scene_file.scene->get_background_color() * path_vertex.attenuation;
    }
  }
  return color;
}

void render_buckets(const SceneFile* scene_file,
                    std::shared_ptr<Output> output,
                    BucketRendering* bucket_rendering,
                    Signals* signals_receiver,
                    Signals* signals_sender,
                    uint32_t thread_idx)
{
  ImageBucket bucket;
  while (bucket_rendering->get_next_bucket(bucket))
  {
    for (uint32_t y = bucket.min.y; y < bucket.max.y; y++)
    {
      for (uint32_t x = bucket.min.x; x < bucket.max.x; x++)
      {
        if ((*signals_receiver) & SignalFlags::Stop) return;
        Color color = trace(*scene_file, get_camera_coordinates(scene_file->settings.resolution, {x, y}, false));
        // wait if preview is currently updated
        while ((*signals_receiver) & SignalFlags::PreventOutputAccess);
        (*signals_sender) |= SignalFlags::PreventOutputAccess;
        output->set_pixel(x, y, color);
        (*signals_sender) &= ~SignalFlags::PreventOutputAccess;
      }
    }
  }
  (*signals_sender) |= SignalFlags::Done;
}

void whitted_ray_trace(const WhittedSettings& whitted_settings,
                       const SceneFile& scene_file,
                       std::shared_ptr<Output> output,
                       Signals* master_signals,
                       std::vector<Signals*>* thread_signals,
                       uint32_t thread_count)
{
  BucketRendering bucket_rendering(scene_file);
  std::vector<std::jthread> threads;
  for (uint32_t i = 0; i < thread_count; i++) threads.push_back(std::jthread(&render_buckets, &scene_file, output, &bucket_rendering, master_signals, (*thread_signals)[i], i));
}
