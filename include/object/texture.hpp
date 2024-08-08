#pragma once

#include <vector>
#include <memory>
#include "util/vec2.hpp"
#include "util/vec3.hpp"

enum class TextureType
{
  Albedo,
  Edges,
  Checker,
  Bitmap
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
    std::vector<uint32_t> bitmap;
    glm::uvec2 resolution;
  };

  struct CheckerParameters
  {
    std::shared_ptr<Texture> even;
    std::shared_ptr<Texture> odd;
    float tile_size;
  };

  struct EdgesParameters
  {
    std::shared_ptr<Texture> edge;
    std::shared_ptr<Texture> center;
    float thickness;
  };

  Texture(const glm::vec3& albedo);
  Texture(const AlbedoParameters& albedo_params);
  Texture(const BitmapParameters& bitmap_params);
  Texture(const CheckerParameters& checker_params);
  Texture(const EdgesParameters& edges_params);
  ~Texture();
  glm::vec3 get_value(glm::vec2 bary, glm::vec2 tex_coords) const;

private:
  TextureType type;
  union
  {
    AlbedoParameters albedo_params;
    BitmapParameters bitmap_params;
    CheckerParameters checker_params;
    EdgesParameters edges_params;
  };
};
