cmake_minimum_required(VERSION 3.19)
project(Ray_Tracer_CPP)

set(CMAKE_CXX_STANDARD 20)

add_executable(Ray_Tracer_CPP main.cpp Ray.h Camera.h HitRecord.h
        objects/Hitable.h objects/Sphere.cpp objects/Sphere.h objects/HitableList.cpp objects/HitableList.h
        materials/Material.h materials/Lambertian.cpp materials/Lambertian.h materials/Metal.cpp materials/Metal.h materials/Dielectric.cpp materials/Dielectric.h
        sdl_window/Color.cpp sdl_window/Color.h sdl_window/Renderer.cpp sdl_window/Renderer.h)

find_library(sdl SDL2.lib "C:/dev/VulkanSDK/1.2.176.1/Third-Party/Bin/")

target_link_libraries(Ray_Tracer_CPP
        ${sdl})

target_include_directories(Ray_Tracer_CPP PUBLIC
        "${PROJECT_BINARY_DIR}"
        "${PROJECT_SOURCE_DIR}"
        "C:/dev/VulkanSDK/1.2.176.1/Third-Party/Include/glm"
        "C:/dev/VulkanSDK/1.2.176.1/Third-Party/Include/SDL2"
        "C:/dev/stb/01.04/")

configure_file("C:/dev/VulkanSDK/1.2.176.1/Third-Party/Bin/SDL2.dll"
        "${PROJECT_SOURCE_DIR}/cmake-build-debug/" COPYONLY)

configure_file("C:/dev/VulkanSDK/1.2.176.1/Third-Party/Bin/SDL2.dll"
        "${PROJECT_SOURCE_DIR}/cmake-build-release/" COPYONLY)