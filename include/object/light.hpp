#pragma once

#include "util/vec3.hpp"

class Light
{
public:
  Light(float intensity, const cm::Vec3& position);
  cm::Vec3 get_contribution(const cm::Vec3& pos, const cm::Vec3& normal);
  float get_intensity() const;
  cm::Vec3 get_position() const;
  void set_position(const cm::Vec3& position);
  float set_intensity(float new_intensity);

private:
  float intensity;
  cm::Vec3 position;
};

Light interpolate(const Light& a, const Light& b, float weight);
