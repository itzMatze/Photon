#include "object/light.hpp"

Light::Light(float intensity, const cm::Vec3& position) : intensity(intensity), position(position)
{}

cm::Vec3 Light::get_contribution(const cm::Vec3& pos, const cm::Vec3& normal)
{
  cm::Vec3 contribution(0.0);
  const float distance = cm::length(position - pos);
  contribution = cm::Vec3((intensity * std::max(0.0f, cm::dot(position - pos, normal))) / (distance * distance));
  return contribution;
}

float Light::get_intensity() const
{
  return intensity;
}

cm::Vec3 Light::get_position() const
{
  return position;
}

void Light::set_position(const cm::Vec3& position)
{
  this->position = position;
}

float Light::set_intensity(float new_intensity)
{
  return intensity = new_intensity;
}

Light interpolate(const Light& a, const Light& b, float weight)
{
  cm::Vec3 pos = (1 - weight) * a.get_position() + weight * b.get_position();
  float intensity = (1 - weight) * a.get_intensity() + weight * b.get_intensity();
  return Light(intensity, pos);
}
