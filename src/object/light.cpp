#include "object/light.hpp"
#include <glm/geometric.hpp>

Light::Light(float intensity, const glm::vec3& position) : intensity(intensity), position(position)
{}

glm::vec3 Light::get_contribution(const glm::vec3& pos, const glm::vec3& normal)
{
  glm::vec3 contribution(0.0);
  const float distance = glm::length(position - pos);
  contribution = glm::vec3((intensity * std::max(0.0f, glm::dot(position - pos, normal))) / (distance * distance));
  return contribution;
}

float Light::get_intensity() const
{
  return intensity;
}

glm::vec3 Light::get_position() const
{
  return position;
}

void Light::set_position(const glm::vec3& position)
{
  this->position = position;
}

float Light::set_intensity(float new_intensity)
{
  return intensity = new_intensity;
}

Light interpolate(const Light& a, const Light& b, float weight)
{
  glm::vec3 pos = (1 - weight) * a.get_position() + weight * b.get_position();
  float intensity = (1 - weight) * a.get_intensity() + weight * b.get_intensity();
  return Light(intensity, pos);
}