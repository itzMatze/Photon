cmake_minimum_required(VERSION 3.19)
project(Ray_Tracer_CPP)
set(CMAKE_CXX_STANDARD 20)

set(glm_DIR dependencies/glm-0.9.9.8/glm/cmake/glm)
set(stb_dir dependencies/stb-master_af1a5bc_10.09.21)
set(ImGui_dir dependencies/imgui-1.87)
set(SDL2_DIR dependencies/SDL2-2.0.20)
set(google_test_DIR dependencies/googletest-release-1.11.0)

set(SOURCE_FILES src/RayTracer.cpp src/RandomGenerator.cpp src/Factory.cpp src/Perlin.cpp src/Materials.cpp
        src/objects/Sphere.cpp src/objects/HitableList.cpp src/objects/Segment.cpp src/objects/Aabb.cpp src/objects/Bvh.cpp src/objects/Triangle.cpp
        src/sdl_window/Color.cpp src/sdl_window/Renderer.cpp
        ${ImGui_dir}/imgui.cpp ${ImGui_dir}/imgui_draw.cpp ${ImGui_dir}/imgui_tables.cpp
        ${ImGui_dir}/imgui_widgets.cpp ${ImGui_dir}/backends/imgui_impl_sdl.cpp ${ImGui_dir}/backends/imgui_impl_sdlrenderer.cpp)

add_executable(Ray_Tracer_CPP src/main.cpp ${SOURCE_FILES})

find_package(glm REQUIRED)
find_package(SDL2 REQUIRED)

find_library(SDL2_LIB SDL2.lib ${SDL2_DIR}/lib/x64/)

target_link_libraries(Ray_Tracer_CPP ${SDL2_LIBRARIES})
include_directories(Ray_Tracer_CPP PUBLIC
        "${PROJECT_SOURCE_DIR}/include"
        ${SDL2_INCLUDE_DIRS}
        "${stb_dir}"
        "${ImGui_dir}"
        ${GLM_INCLUDE_DIRS})

if (WIN32)
    configure_file("${SDL2_LIBRARIES_DIR}/SDL2.dll" "${CMAKE_BINARY_DIR}/" COPYONLY)
endif()

if (CMAKE_BUILD_TYPE MATCHES Debug)
        enable_testing()

        add_subdirectory(${google_test_DIR} ${CMAKE_BINARY_DIR}/googletest EXCLUDE_FROM_ALL)

        add_executable(run_google_test google_test/ColorTest.cpp ${SOURCE_FILES})
        target_link_libraries(run_google_test gtest_main ${SDL2_LIBRARIES})

        include(GoogleTest)
        gtest_discover_tests(run_google_test)
endif()
