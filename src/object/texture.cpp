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

Texture::Texture(const EdgesParameters& edges_params) : type(TextureType::Edges), edges_params(edges_params)
{}

Texture::~Texture()
{
  switch (type)
  {
    case TextureType::Albedo:
      return;
    case TextureType::Bitmap:
      return;
    case TextureType::Checker:
      checker_params.even = nullptr;
      checker_params.odd = nullptr;
      return;
    case TextureType::Edges:
      edges_params.edge = nullptr;
      edges_params.center = nullptr;
      return;
  }
}

glm::vec3 Texture::get_value(glm::vec2 bary, glm::vec2 tex_coords) const
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
      Color color(bitmap_params.bitmap[idx.y * bitmap_params.resolution.x + idx.x]);
      return color.value;
    }
    case TextureType::Checker:
    {
      glm::uvec2 tile_idx = tex_coords / checker_params.tile_size;
      // check whether both indices are either even or odd, then the even texture is used
      // modulo check with bit twiddling
      if ((tile_idx.x & 1u) == (tile_idx.y & 1))
      {
        return checker_params.even->get_value(bary, tex_coords);
      }
      else
      {
        return checker_params.odd->get_value(bary, tex_coords);
      }
    }
    case TextureType::Edges:
    {
      // check if point is on the edge
      if (bary.s < edges_params.thickness || bary.t < edges_params.thickness || (1 - bary.s - bary.t) < edges_params.thickness)
      {
        return edges_params.edge->get_value(bary, tex_coords);
      }
      else
      {
        return edges_params.center->get_value(bary, tex_coords);
      }
    }
  }
  // should never be reached
  return glm::vec3(0.0, 0.0, 0.0);
}
