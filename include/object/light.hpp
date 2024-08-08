#pragma once

#include "util/vec3.hpp"

class Light
{
public:
  Light(float intensity, const glm::vec3& position);
  glm::vec3 get_contribution(const glm::vec3& pos, const glm::vec3& normal);
  float get_intensity() const;
  glm::vec3 get_position() const;
  void set_position(const glm::vec3& position);
  float set_intensity(float new_intensity);

private:
  float intensity;
  glm::vec3 position;
};

Light interpolate(const Light& a, const Light& b, float weight);
