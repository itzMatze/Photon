#pragma once
#include <cstdint>
#include <memory>
#include <vector>
#include "object/light.hpp"
#include "renderer/camera.hpp"
#include "renderer/color.hpp"
#include "scene/geometry.hpp"
#include "util/interpolatable_data.hpp"

class Scene
{
public:
  Scene() = default;
  Scene(const std::vector<std::shared_ptr<Geometry>>& geometry_keyframes,
        const std::vector<std::shared_ptr<InterpolatableData<Light>>>& light_keyframes,
        const std::vector<std::shared_ptr<CameraConfig>>& camera_keyframes,
        const std::vector<uint32_t>& frame_counts,
        const Color& background_color);

  Color get_background_color() const;
  const Geometry& get_geometry() const;
  const std::vector<Light>& get_lights() const;
  const Camera& get_camera() const;
  bool step();
  bool is_animated() const;

private:
  std::vector<std::shared_ptr<Geometry>> geometry_keyframes;
  std::vector<std::shared_ptr<InterpolatableData<Light>>> light_keyframes;
  std::vector<std::shared_ptr<CameraConfig>> camera_keyframes;
  std::vector<uint32_t> frame_counts;
  std::shared_ptr<Geometry> current_geometry;
  std::vector<Light> current_lights;
  Camera current_camera;
  Color background_color;
  uint32_t current_keyframe;
  uint32_t current_frame_step;
};

