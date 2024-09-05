#include "scene/scene.hpp"
#include <cassert>

Scene::Scene(const std::vector<std::shared_ptr<Geometry>>& geometry_keyframes,
             const std::vector<std::shared_ptr<InterpolatableData<Light>>>& light_keyframes,
             const std::vector<std::shared_ptr<CameraConfig>>& camera_keyframes,
             const std::vector<uint32_t>& frame_counts,
             const Color& background_color) :
             geometry_keyframes(geometry_keyframes),
             light_keyframes(light_keyframes),
             camera_keyframes(camera_keyframes),
             frame_counts(frame_counts),
             background_color(background_color),
             current_keyframe(0),
             current_frame_step(0)
{
  assert(geometry_keyframes.size() == camera_keyframes.size() && geometry_keyframes.size() == (frame_counts.size() + 1));
  assert(geometry_keyframes.size() > 0 && camera_keyframes.size() > 0);
  current_geometry = geometry_keyframes[0];
  current_lights = light_keyframes[0]->get_data();
  current_camera = Camera(*camera_keyframes[0]);
}

Color Scene::get_background_color() const
{
  return background_color;
}

const Geometry& Scene::get_geometry() const
{
  return *current_geometry;
}

const std::vector<Light>& Scene::get_lights() const
{
  return current_lights;
}

const Camera& Scene::get_camera() const
{
  return current_camera;
}

bool Scene::step()
{
  // if no keyframe is left return false
  if (current_keyframe >= frame_counts.size()) return false;
  uint32_t frame_count = frame_counts[current_keyframe];

  // interpolation weight between current and next keyframe
  float weight = float(current_frame_step) / float(frame_count);
  current_geometry = std::make_shared<Geometry>(interpolate(*geometry_keyframes[current_keyframe], *geometry_keyframes[current_keyframe + 1], weight));
  current_lights = interpolate(*light_keyframes[current_keyframe], *light_keyframes[current_keyframe + 1], weight).get_data();
  CameraConfig interpolated_cam = interpolate(*camera_keyframes[current_keyframe], *camera_keyframes[current_keyframe + 1], weight);
  current_camera = Camera(interpolated_cam);

  // if all frames between the current and the next keyframe are rendered, move to the next keyframe
  if (++current_frame_step > frame_count)
  {
    current_keyframe++;
    current_frame_step = 1;
  }
  return true;
}

bool Scene::is_animated() const
{
  return frame_counts.size() > 0;
}

