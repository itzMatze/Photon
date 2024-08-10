#pragma once

#include "color.hpp"
#include "scene/scene_file_handler.hpp"
#include "util/vec2.hpp"

Color whitted_ray_trace(const SceneFile& scene_file, glm::vec2 camera_coordinates);
