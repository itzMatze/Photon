cmake_minimum_required(VERSION 3.19)
project(Photon)
set(CMAKE_CXX_STANDARD 20)

set(SDL2_DIR "${PROJECT_SOURCE_DIR}/dependencies/SDL2")
set(SPDLOG_DIR "${PROJECT_SOURCE_DIR}/dependencies/spdlog")

set(SOURCE_FILES
  src/image/bitmap.cpp
  src/image/image_file_handler.cpp
  src/object/aabb.cpp
  src/object/bvh.cpp
  src/object/light.cpp
  src/object/material.cpp
  src/object/model.cpp
  src/object/object.cpp
  src/object/object_instance.cpp
  src/object/texture.cpp
  src/object/triangle.cpp
  src/renderer/bucket_rendering.cpp
  src/renderer/camera.cpp
  src/renderer/color.cpp
  src/renderer/output.cpp
  src/renderer/ray.cpp
  src/renderer/renderer.cpp
  src/renderer/whitted_ray_tracer.cpp
  src/renderer/window.cpp
  src/scene/geometry.cpp
  src/scene/geometry_builder.cpp
  src/scene/scene.cpp
  src/scene/scene_factory.cpp
  src/scene/scene_builder.cpp
  src/scene/scene_file_handler.cpp
  src/util/spatial_configuration.cpp
  src/util/random_generator.cpp
  dependencies/stb/stb_image.cpp
  dependencies/stb/stb_image_write.cpp
  dependencies/tinygltf/tinygltf.cpp)

add_executable(Photon src/main.cpp ${SOURCE_FILES})

target_include_directories(Photon PRIVATE "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_SOURCE_DIR}/dependencies/"
  "${SDL2_DIR}/include",
  "${SPDLOG_DIR}/include")

add_subdirectory("${SDL2_DIR}")
target_link_libraries(Photon SDL2)

add_subdirectory("${SPDLOG_DIR}")
target_link_libraries(Photon spdlog::spdlog)
