#include "scene/scene_builder.hpp"
#include <memory>
#include <vector>
#include "renderer/camera.hpp"
#include "util/interpolatable_data.hpp"

SceneBuilder::SceneBuilder()
{
  geometry_keyframes.push_back(std::make_shared<GeometryBuilder>());
  light_keyframes.push_back(std::make_shared<InterpolatableData<Light>>());
  camera_keyframes.push_back(std::make_shared<CameraConfig>());
}

void SceneBuilder::new_keyframe(uint32_t frame_count)
{
  frame_counts.emplace_back(frame_count);
  // store next keyframe as pointer to last keyframe
  // copy keyframe in non-const getter because then the user wants to change it
  geometry_keyframes.emplace_back(geometry_keyframes.back());
  light_keyframes.emplace_back(light_keyframes.back());
  camera_keyframes.emplace_back(camera_keyframes.back());
}

void SceneBuilder::set_background(const Color& color)
{
  background_color = color;
}

const GeometryBuilder& SceneBuilder::get_geometry() const
{
  return *geometry_keyframes.back();
}

GeometryBuilder& SceneBuilder::get_geometry()
{
  if (geometry_keyframes.back().use_count() != 1)
  {
    // copy keyframe so it can be edited
    geometry_keyframes.back() = std::make_shared<GeometryBuilder>(*(geometry_keyframes.back()));
  }
  return *geometry_keyframes.back();
}

const InterpolatableData<Light>& SceneBuilder::get_lights() const
{
  return *light_keyframes.back();
}

InterpolatableData<Light>& SceneBuilder::get_lights()
{
  if (light_keyframes.back().use_count() != 1)
  {
    // copy keyframe so it can be edited
    light_keyframes.back() = std::make_shared<InterpolatableData<Light>>(*(light_keyframes.back()));
  }
  return *light_keyframes.back();
}

const CameraConfig& SceneBuilder::get_camera() const
{
  return *camera_keyframes.back();
}

CameraConfig& SceneBuilder::get_camera()
{
  if (camera_keyframes.back().use_count() != 1)
  {
    // copy keyframe so it can be edited
    camera_keyframes.back() = std::make_shared<CameraConfig>(*(camera_keyframes.back()));
  }
  return *camera_keyframes.back();
}

std::shared_ptr<Scene> SceneBuilder::build_scene()
{
  std::vector<std::shared_ptr<Geometry>> geometries;
  for (const auto& geometry : geometry_keyframes) geometries.push_back(std::make_shared<Geometry>(geometry->build_geometry()));
  return std::make_shared<Scene>(geometries, light_keyframes, camera_keyframes, frame_counts, background_color);
}

