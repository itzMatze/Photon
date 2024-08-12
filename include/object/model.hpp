#pragma once

#include "scene/scene_builder.hpp"

namespace GLTFModel
{
  int load(SceneBuilder& scene_builder, const std::string& model_path, bool load_materials = true);
};
