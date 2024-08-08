#include "renderer/ray.hpp"

Ray::Ray(const glm::vec3& origin, const glm::vec3& dir, const RayConfig config) : origin(origin), dir(dir), inv_dir(1.0f / this->dir), config(config)
{}

glm::vec3 Ray::at(float t) const
{
  return origin + t * dir;
}

const glm::vec3& Ray::get_dir() const
{
  return dir;
}

const glm::vec3& Ray::get_inv_dir() const
{
  return inv_dir;
}

void Ray::set_dir(const glm::vec3& new_dir)
{
  dir = new_dir;
  inv_dir = 1.0f / new_dir;
}
