#include "renderer/rendering_algorithms.hpp"

Color whitted_ray_trace(const SceneFile &scene_file, glm::vec2 camera_coordinates)
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
      const Material& material = scene_file.scene->get_geometry().get_materials()[hit_info.material_idx];
      // whitted ray tracing can only handle perfectly transmissive, perfectly reflective, and diffuse materials
      if (material.is_delta())
      {
        const uint32_t depth = path_vertex.depth + 1;
        if (depth < scene_file.settings.max_path_length)
        {
          bsdf_samples.clear();
          material.get_bsdf_samples(hit_info, path_vertex.ray.get_dir(), bsdf_samples, scene_file.scene->get_geometry().get_textures());
          for (const auto& bsdf_sample : bsdf_samples)
          {
            const PathVertex next_path_vertex = PathVertex{bsdf_sample.ray, path_vertex.attenuation * bsdf_sample.attenuation, depth};
            path_vertices.push_back(next_path_vertex);
          }
        }
      }
      else
      {
        // if material does not depend on light (usually debug vis) just fetch albedo
        if (!material.is_light_dependent())
        {
          color.value += material.get_albedo(hit_info, scene_file.scene->get_geometry().get_textures());
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
            contribution *= path_vertex.attenuation * material.eval(hit_info, path_vertex.ray.get_dir(), outgoing_dir, scene_file.scene->get_geometry().get_textures());
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
  return color;
}
