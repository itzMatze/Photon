#pragma once

#include <vector>
#include "image/bitmap.hpp"
#include "util/vec2.hpp"
#include "util/vec3.hpp"

enum class TextureType
{
  Albedo,
  Bitmap,
  Checker
};

class Texture
{
public:
  struct AlbedoParameters
  {
    glm::vec3 albedo;
  };

  struct BitmapParameters
  {
    int32_t bitmap_idx;
    glm::uvec2 resolution;
  };

  struct CheckerParameters
  {
    int32_t even_texture_idx;
    int32_t odd_texture_idx;
    float tile_size;
  };

  Texture(const glm::vec3& albedo);
  Texture(const AlbedoParameters& albedo_params);
  Texture(const BitmapParameters& bitmap_params);
  Texture(const CheckerParameters& checker_params);
  ~Texture();
  glm::vec3 get_value(glm::vec2 bary, glm::vec2 tex_coords, const std::vector<Texture>& textures, const std::vector<Bitmap>& bitmaps) const;

private:
  TextureType type;
  union
  {
    AlbedoParameters albedo_params;
    BitmapParameters bitmap_params;
    CheckerParameters checker_params;
  };
};
