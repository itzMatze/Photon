cmake_minimum_required(VERSION 3.19)
project(Ray_Tracer_CPP)
set(CMAKE_CXX_STANDARD 20)
add_subdirectory(google_test)

set(glm_dir X:/Stuff/VulkanSDK/1.2.176.1/Third-Party/Include/glm)
set(stb_dir X:/Stuff/stb/01.04/)
# using unreleased versions of SDL and ImGui
set(ImGui_dir X:/Stuff/imgui/imgui-sdlrenderer)
set(SDL_dir X:/Stuff/SDL2/SDL-main)

add_executable(Ray_Tracer_CPP main.cpp Ray.h Camera.h HitRecord.h
        objects/Hitable.h objects/Sphere.cpp objects/Sphere.h objects/HitableList.cpp objects/HitableList.h objects/Bezier.cpp objects/Bezier.h
        materials/Material.h materials/Lambertian.cpp materials/Lambertian.h materials/Metal.cpp materials/Metal.h materials/Dielectric.cpp materials/Dielectric.h
        sdl_window/Color.cpp sdl_window/Color.h sdl_window/Renderer.cpp sdl_window/Renderer.h RandomGenerator.cpp RandomGenerator.h Factory.cpp Factory.h
        ${ImGui_dir}/imgui.cpp ${ImGui_dir}/imgui_draw.cpp ${ImGui_dir}/imgui_tables.cpp ${ImGui_dir}/imgui_widgets.cpp ${ImGui_dir}/backends/imgui_impl_sdl.cpp ${ImGui_dir}/backends/imgui_impl_sdlrenderer.cpp)

find_library(sdl SDL2.lib "${SDL_dir}/cmake-build-release/")

target_link_libraries(Ray_Tracer_CPP ${sdl})

include_directories(Ray_Tracer_CPP PUBLIC
        "${PROJECT_BINARY_DIR}"
        "${PROJECT_SOURCE_DIR}"
        "${glm_dir}"
        "${SDL_dir}/include"
        "${stb_dir}"
        "${ImGui_dir}")

configure_file("${SDL_dir}/cmake-build-release/SDL2.dll"
        "${PROJECT_SOURCE_DIR}/cmake-build-debug/" COPYONLY)

configure_file("${SDL_dir}/cmake-build-release/SDL2.dll"
        "${PROJECT_SOURCE_DIR}/cmake-build-release/" COPYONLY)
