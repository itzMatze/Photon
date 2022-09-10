#pragma once

#include <filesystem>

#include "RandomGenerator.h"
#include "objects/HitableList.h"

void save_image(uint32_t* pixels, const std::string& name, int nx, int ny, int channels);
void load_obj(std::string file, glm::mat4x4 transformation);
HitableList random_scene(RandomGenerator* random_generator);
HitableList create_scene();
HitableList line_scene();
