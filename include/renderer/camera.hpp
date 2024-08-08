#pragma once
#include "renderer/ray.hpp"
#include "util/spatial_configuration.hpp"
#include "util/vec2.hpp"
#include "util/vec3.hpp"

class CameraConfig
{
public:
  CameraConfig();

  SpatialConfiguration& get_spatial_conf();
  const SpatialConfiguration& get_spatial_conf() const;
  float get_focal_length() const;
  void set_focal_length(float new_focal_length);
  float get_sensor_size() const;
  void set_sensor_size(float new_sensor_size);

private:
  SpatialConfiguration spatial_conf;
  float focal_length;
  // sensor is quadratic
  float sensor_size;
};

CameraConfig interpolate(const CameraConfig& a, const CameraConfig& b, float weight);

/*
 * ray tracing camera that can generate rays for each pixel
 * resolution of the image that the rays are generated for
 * orientation and parameters of the camera
*/
class Camera
{
public:
  Camera(const CameraConfig& config = CameraConfig());

  // camera acts as a quadratic texture that can be accessed texture coordinates
  // sensor is defined in [0,1], but accesses outside are still valid
  Ray get_ray(const glm::vec2 pixel) const;

private:
  SpatialConfiguration spatial_conf;
  // position of the camera sensor
  glm::vec3 upper_left_corner;
  // sensor is quadratic
  float sensor_size;
};

