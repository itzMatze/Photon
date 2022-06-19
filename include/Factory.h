#pragma once

#include <filesystem>

#include "materials/Dielectric.h"
#include "materials/Metal.h"
#include "materials/Lambertian.h"
#include "objects/Sphere.h"
#include "objects/HitableList.h"
#include "objects/Hitable.h"

void save_image(uint32_t* pixels, const std::string& name, int nx, int ny, int channels);
std::shared_ptr<HitableList> random_scene(RandomGenerator* random_generator);
std::shared_ptr<HitableList> create_scene();
std::shared_ptr<HitableList> line_scene();
