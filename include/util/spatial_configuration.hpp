#pragma once

#include "glm/matrix.hpp"
#include "util/vec3.hpp"
#include "glm/gtc/quaternion.hpp"

constexpr glm::vec3 x_axis(1.0, 0.0, 0.0);
constexpr glm::vec3 y_axis(0.0, 1.0, 0.0);
constexpr glm::vec3 z_axis(0.0, 0.0, 1.0);

class SpatialConfiguration
{
public:
  SpatialConfiguration();
  SpatialConfiguration(const glm::mat3& orientation, const glm::vec3& position = glm::vec3(0.0, 0.0, 0.0), float scale = 1.0);
  SpatialConfiguration(const glm::quat& orientation, const glm::vec3& position = glm::vec3(0.0, 0.0, 0.0), float scale = 1.0);
  SpatialConfiguration(const glm::vec3& position);
  void translate(const glm::vec3& translation);
  void set_position(const glm::vec3& position);
  void rotate(glm::vec3 angles, bool use_radian = false);
  void rotate(float yaw, float pitch, float roll, bool use_radian = false);
  void set_orientation(const glm::mat3& orientation);
  void set_orientation(const glm::quat& orientation);
  void set_scale(float scale);
  glm::vec3 get_position() const;
  glm::quat get_orientation() const;
  float get_scale() const;
  glm::vec3 get_x_axis() const;
  glm::vec3 get_y_axis() const;
  glm::vec3 get_z_axis() const;
  glm::vec3 transform_pos(const glm::vec3& pos) const;
  glm::vec3 transform_dir(const glm::vec3& dir) const;
  glm::vec3 inverse_transform_pos(const glm::vec3& pos) const;
  glm::vec3 inverse_transform_dir(const glm::vec3& dir) const;

private:
  glm::quat orientation;
  glm::vec3 position;
  float scale;
  // coordinate system
  glm::vec3 local_x_axis, local_y_axis, local_z_axis;

  void update_coordinate_system();
};

SpatialConfiguration interpolate(const SpatialConfiguration& a, const SpatialConfiguration& b, float weight);
