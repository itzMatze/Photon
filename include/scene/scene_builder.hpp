#pragma once
#include <memory>
#include <vector>
#include "object/light.hpp"
#include "renderer/camera.hpp"
#include "renderer/color.hpp"
#include "scene/geometry_builder.hpp"
#include "scene/scene.hpp"
#include "util/interpolatable_data.hpp"

class SceneBuilder
{
public:
  SceneBuilder();

  void new_keyframe(uint32_t frame_count /* number of frames between previous and new keyframe */);

  const GeometryBuilder& get_geometry() const;
  GeometryBuilder& get_geometry();

  const InterpolatableData<Light>& get_lights() const;
  InterpolatableData<Light>& get_lights();

  const CameraConfig& get_camera() const;
  CameraConfig& get_camera();

  void set_background(const Color& color);
  Scene build_scene();

private:
  std::vector<std::shared_ptr<GeometryBuilder>> geometry_keyframes;
  std::vector<std::shared_ptr<InterpolatableData<Light>>> light_keyframes;
  std::vector<std::shared_ptr<CameraConfig>> camera_keyframes;
  std::vector<uint32_t> frame_counts;
  Color background_color;
};

