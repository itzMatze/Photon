cmake_minimum_required(VERSION 3.19)
project(Photon)
set(CMAKE_CXX_STANDARD 20)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_compile_options(-msse4.2)
  add_compile_definitions(SSE_ENABLED)
endif()

set(SOURCE_FILES
  src/image/image_file_handler.cpp
  src/object/aabb.cpp
  src/object/bvh.cpp
  src/object/geometry.cpp
  src/object/geometry_builder.cpp
  src/object/light.cpp
  src/object/material.cpp
  src/object/object.cpp
  src/object/texture.cpp
  src/object/triangle.cpp
  src/renderer/camera.cpp
  src/renderer/color.cpp
  src/renderer/ray.cpp
  src/renderer/renderer.cpp
  src/scene/scene.cpp
  src/scene/scene_factory.cpp
  src/scene/scene_builder.cpp
  src/scene/scene_file_handler.cpp
  src/util/spatial_configuration.cpp
  src/util/random_generator.cpp
  dependencies/stb/stb_image.cpp
  dependencies/stb/stb_image_write.cpp)

add_executable(Photon src/main.cpp ${SOURCE_FILES})
target_include_directories(Photon PRIVATE "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_SOURCE_DIR}/dependencies/")
