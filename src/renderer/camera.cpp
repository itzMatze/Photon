#include "renderer/camera.hpp"
#include "util/log.hpp"
#include "util/random_generator.hpp"
#include "util/spatial_configuration.hpp"
#include "util/vec3.hpp"

CameraConfig::CameraConfig() :
  spatial_conf(glm::vec3(0.0, 0.0, 0.0)),
  focal_length(0.03) /* 30mm camera lens */,
  sensor_size(0.036) /* standard full frame sensor width 36mm */
{}

SpatialConfiguration& CameraConfig::get_spatial_conf() { return spatial_conf; }

const SpatialConfiguration& CameraConfig::get_spatial_conf() const { return spatial_conf; }

float CameraConfig::get_focal_length() const { return focal_length; }

void CameraConfig::set_focal_length(float new_focal_length) { focal_length = new_focal_length; }

float CameraConfig::get_sensor_size() const { return sensor_size; }

void CameraConfig::set_sensor_size(float new_sensor_size) { sensor_size = new_sensor_size; }

CameraConfig interpolate(const CameraConfig& a, const CameraConfig& b, float weight)
{
  CameraConfig result;
  result.get_spatial_conf() = interpolate(a.get_spatial_conf(), b.get_spatial_conf(), weight);
  result.set_focal_length((1 - weight) * a.get_focal_length() + weight * b.get_focal_length());
  result.set_sensor_size((1 - weight) * a.get_sensor_size() + weight * b.get_sensor_size());
  return result;
}

Camera::Camera(const CameraConfig& config)
  : spatial_conf(config.get_spatial_conf()), sensor_size(config.get_sensor_size())
{
  // camera coordinate system
  upper_left_corner = spatial_conf.get_position() + (sensor_size / 2.0f) * spatial_conf.get_y_axis() - (sensor_size / 2.0f) * spatial_conf.get_x_axis() - config.get_focal_length() * spatial_conf.get_z_axis();
}

Ray Camera::get_ray(const glm::vec2 pixel) const
{
  const glm::vec3 pixel_pos = upper_left_corner + (pixel.x * sensor_size * spatial_conf.get_x_axis()) - (pixel.y * sensor_size * spatial_conf.get_y_axis());
  return Ray(pixel_pos, glm::normalize(pixel_pos - spatial_conf.get_position()));
}

glm::vec2 get_camera_coordinates(glm::uvec2 resolution, glm::uvec2 pixel, bool use_jittering, RandomGenerator* rnd)
{
  PH_ASSERT(!use_jittering || (use_jittering && rnd), "Pass a random generator if jittering is enabled!");
  // offset to either get a random position inside of the pixel square or the center of the pixel
  glm::vec2 offset = use_jittering ? glm::vec2(rnd->random_float(), rnd->random_float()) : glm::vec2(0.5);
  glm::vec2 pixel_coordinates = (glm::vec2(pixel) + offset) / glm::vec2(resolution);
  float aspect_ratio = float(resolution.y) / float(resolution.x);
  pixel_coordinates.y *= aspect_ratio;
  pixel_coordinates.y += (1.0 - aspect_ratio) / 2.0;
  return pixel_coordinates;
}
