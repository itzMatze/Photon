#include "util/spatial_configuration.hpp"

SpatialConfiguration::SpatialConfiguration() : orientation(glm::quat(1.0, 0.0, 0.0, 0.0)), position(glm::vec3(0.0, 0.0, 0.0)), scale(1.0)
{
  update_coordinate_system();
}

SpatialConfiguration::SpatialConfiguration(const glm::mat3& orientation, const glm::vec3& position, float scale) : orientation(glm::quat_cast(orientation)), position(position), scale(scale)
{
  update_coordinate_system();
}

SpatialConfiguration::SpatialConfiguration(const glm::quat& orientation, const glm::vec3& position, float scale) : orientation(orientation), position(position), scale(scale)
{
  update_coordinate_system();
}

SpatialConfiguration::SpatialConfiguration(const glm::vec3& position) : orientation(glm::quat(1.0, 0.0, 0.0, 0.0)), position(position), scale(1.0)
{
  update_coordinate_system();
}

void SpatialConfiguration::translate(const glm::vec3& translation)
{
  position += translation;
}

void SpatialConfiguration::set_position(const glm::vec3& position)
{
  this->position = position;
}

void SpatialConfiguration::rotate(glm::vec3 angles, bool use_radian)
{
  rotate(angles.x, angles.y, angles.z, use_radian);
}

void SpatialConfiguration::rotate(float yaw, float pitch, float roll, bool use_radian)
{
  // calculate incremental change in angles with respect to current coordinate system
  if (!use_radian)
  {
    yaw = yaw * (M_PI / 180.0);
    pitch = pitch * (M_PI / 180.0);
    roll = roll * (M_PI / 180.0);
  }
  glm::quat q_yaw = glm::angleAxis(yaw, local_y_axis);
  glm::quat q_pitch = glm::angleAxis(pitch, local_x_axis);
  glm::quat q_roll = glm::angleAxis(roll, local_z_axis);

  // apply incremental change to orientation
  orientation = glm::normalize(q_yaw * q_pitch * q_roll * orientation);
  update_coordinate_system();
}

void SpatialConfiguration::set_orientation(const glm::mat3& orientation)
{
  this->orientation = glm::quat_cast(orientation);
  update_coordinate_system();
}

void SpatialConfiguration::set_orientation(const glm::quat& orientation)
{
  this->orientation = orientation;
  update_coordinate_system();
}

void SpatialConfiguration::set_scale(float scale)
{
  this->scale = scale;
}

glm::vec3 SpatialConfiguration::get_position() const
{
  return position;
}

glm::quat SpatialConfiguration::get_orientation() const
{
  return orientation;
}

float SpatialConfiguration::get_scale() const
{
  return scale;
}

glm::vec3 SpatialConfiguration::get_x_axis() const
{
  return local_x_axis;
}

glm::vec3 SpatialConfiguration::get_y_axis() const
{
  return local_y_axis;
}

glm::vec3 SpatialConfiguration::get_z_axis() const
{
  return local_z_axis;
}

glm::vec3 SpatialConfiguration::transform_pos(const glm::vec3& pos) const
{
  const glm::vec3 scaled_pos = pos * scale;
  const glm::quat quat_pos = orientation * glm::quat(0.0, scaled_pos.x, scaled_pos.y, scaled_pos.z) * glm::conjugate(orientation);
  return glm::vec3(quat_pos.x, quat_pos.y, quat_pos.z) + position;
}

glm::vec3 SpatialConfiguration::transform_dir(const glm::vec3& dir) const
{
  const glm::quat quat_dir = orientation * glm::quat(0.0, dir.x, dir.y, dir.z) * glm::conjugate(orientation);
  return glm::vec3(quat_dir.x, quat_dir.y, quat_dir.z) * scale;
}

glm::vec3 SpatialConfiguration::inverse_transform_pos(const glm::vec3& pos) const
{
  const glm::vec3 new_pos = pos - position;
  const glm::quat quat_pos = glm::conjugate(orientation) * glm::quat(0.0, new_pos.x, new_pos.y, new_pos.z) * orientation;
  return glm::vec3(quat_pos.x, quat_pos.y, quat_pos.z) / scale;
}

glm::vec3 SpatialConfiguration::inverse_transform_dir(const glm::vec3& dir) const
{
  const glm::quat quat_dir = glm::conjugate(orientation) * glm::quat(0.0, dir.x, dir.y, dir.z) * orientation;
  return glm::vec3(quat_dir.x, quat_dir.y, quat_dir.z) / scale;
}

SpatialConfiguration interpolate(const SpatialConfiguration& a, const SpatialConfiguration& b, float weight)
{
  const glm::quat orientation = glm::slerp(a.get_orientation(), b.get_orientation(), weight);
  const glm::vec3 position = (1 - weight) * a.get_position() + weight * b.get_position();
  const float scale = (1 - weight) * a.get_scale() + weight * b.get_scale();
  return SpatialConfiguration(orientation, position, scale);
}

void SpatialConfiguration::update_coordinate_system()
{
  const glm::quat q_x_axis = glm::normalize(orientation * glm::quat(0.0f, x_axis) * glm::conjugate(orientation));
  const glm::quat q_y_axis = glm::normalize(orientation * glm::quat(0.0f, y_axis) * glm::conjugate(orientation));
  const glm::quat q_z_axis = glm::normalize(orientation * glm::quat(0.0f, z_axis) * glm::conjugate(orientation));
  local_x_axis = glm::normalize(glm::vec3(q_x_axis.x, q_x_axis.y, q_x_axis.z));
  local_y_axis = glm::normalize(glm::vec3(q_y_axis.x, q_y_axis.y, q_y_axis.z));
  local_z_axis = glm::normalize(glm::vec3(q_z_axis.x, q_z_axis.y, q_z_axis.z));
}
