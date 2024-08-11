#include "object/texture.hpp"
#include "renderer/color.hpp"

Texture::Texture(const Bitmap& bitmap) : bitmap(bitmap)
{}

glm::vec3 Texture::get_value(glm::vec2 bary, glm::vec2 tex_coords, const std::vector<Texture>& textures) const
{
  glm::uvec2 idx = tex_coords * glm::vec2(bitmap.get_resolution());
  // invert image because textures are loaded upside down
  idx.y = bitmap.get_resolution().y - idx.y;
  Color color(bitmap.get(idx.x, idx.y));
  return color.value;
}
