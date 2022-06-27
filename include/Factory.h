#pragma once

#include <filesystem>

#include "Materials.h"
#include "Lights.h"
#include "objects/Sphere.h"
#include "objects/HitableList.h"
#include "objects/Hitable.h"

void save_image(uint32_t* pixels, const std::string& name, int nx, int ny, int channels);
HitableList random_scene(RandomGenerator* random_generator);
HitableList create_scene();
HitableList line_scene();
