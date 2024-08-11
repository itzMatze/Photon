#pragma once

#include <vector>
#include "image/bitmap.hpp"
#include "util/vec2.hpp"
#include "util/vec3.hpp"

class Texture
{
public:
  Texture(const Bitmap& bitmap_params);
  glm::vec3 get_value(glm::vec2 bary, glm::vec2 tex_coords, const std::vector<Texture>& textures) const;

private:
  Bitmap bitmap;
};
