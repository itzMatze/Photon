#include "renderer/ray.hpp"

Ray::Ray(const cm::Vec3& origin, const cm::Vec3& dir, const RayConfig config) : origin(origin), dir(dir), inv_dir(1.0f / this->dir), config(config)
{}

cm::Vec3 Ray::at(float t) const
{
  return origin + t * dir;
}

const cm::Vec3& Ray::get_dir() const
{
  return dir;
}

const cm::Vec3& Ray::get_inv_dir() const
{
  return inv_dir;
}

void Ray::set_dir(const cm::Vec3& new_dir)
{
  dir = new_dir;
  inv_dir = 1.0f / new_dir;
}
