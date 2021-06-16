#ifndef RAY_TRACER_CPP_FACTORY_H
#define RAY_TRACER_CPP_FACTORY_H

#include <filesystem>

#include "materials/Dielectric.h"
#include "materials/Metal.h"
#include "materials/Lambertian.h"
#include "objects/Sphere.h"
#include "objects/HitableList.h"
#include "objects/Hitable.h"

void save_image(uint32_t* pixels, const std::string& name, int nx, int ny, int channels);
Hitable* random_scene(RandomGenerator* random_generator);
Hitable* create_scene();

#endif //RAY_TRACER_CPP_FACTORY_H
