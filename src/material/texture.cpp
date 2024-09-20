#include "material/texture.hpp"
#include "glm/common.hpp"
#include "renderer/color.hpp"

Texture::Texture(const Bitmap& bitmap) : bitmap(bitmap)
{}

Color Texture::get_value(glm::vec2 bary, glm::vec2 tex_coords) const
{
  glm::uvec2 idx = tex_coords * glm::vec2(bitmap.get_resolution());
  idx = glm::clamp(idx, glm::uvec2(0.0), bitmap.get_resolution() - 1u);
  return bitmap.get(idx.x, idx.y);
}
