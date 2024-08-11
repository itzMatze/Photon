#include "object/texture.hpp"
#include "renderer/color.hpp"

Texture::Texture(const glm::vec3& albedo) : type(TextureType::Albedo), albedo_params(albedo)
{}

Texture::Texture(const AlbedoParameters& albedo_params) : type(TextureType::Albedo), albedo_params(albedo_params)
{}

Texture::Texture(const BitmapParameters& bitmap_params) : type(TextureType::Bitmap), bitmap_params(bitmap_params)
{}

Texture::Texture(const CheckerParameters& checker_params) : type(TextureType::Checker), checker_params(checker_params)
{}

Texture::~Texture()
{
  return;
}

glm::vec3 Texture::get_value(glm::vec2 bary, glm::vec2 tex_coords, const std::vector<Texture>& textures, const std::vector<Bitmap>& bitmaps) const
{
  switch (type)
  {
    case TextureType::Albedo:
      return albedo_params.albedo;
    case TextureType::Bitmap:
    {
      glm::uvec2 idx = tex_coords * glm::vec2(bitmap_params.resolution);
      // invert image because textures are loaded upside down
      idx.y = bitmap_params.resolution.y - idx.y;
      Color color(bitmaps[bitmap_params.bitmap_idx].get(idx.x, idx.y));
      return color.value;
    }
    case TextureType::Checker:
    {
      glm::uvec2 tile_idx = tex_coords / checker_params.tile_size;
      // check whether both indices are either even or odd, then the even texture is used
      // modulo check with bit twiddling
      if ((tile_idx.x & 1u) == (tile_idx.y & 1))
      {
        return textures[checker_params.even_texture_idx].get_value(bary, tex_coords, textures, bitmaps);
      }
      else
      {
        return textures[checker_params.odd_texture_idx].get_value(bary, tex_coords, textures, bitmaps);
      }
    }
  }
  // should never be reached
  return glm::vec3(0.0, 0.0, 0.0);
}
