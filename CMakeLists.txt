cmake_minimum_required(VERSION 3.19)
project(Ray_Tracer_CPP)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(Ray_Tracer_CPP main.cpp Color.cpp Color.h render/Renderer.cpp render/Renderer.h RayTracer.cpp RayTracer.h)

find_library(glew glew32.lib "C:/dev/glew/glew-2.1.0/lib/Release/x64/")
find_library(sdl SDL2.lib "C:/dev/VulkanSDK/1.2.170.0/Third-Party/Bin/")

target_link_libraries(Ray_Tracer_CPP
        ${glew}
        ${sdl})

configure_file("C:/dev/glew/glew-2.1.0/bin/Release/x64/glew32.dll"
        "${PROJECT_SOURCE_DIR}/cmake-build-debug/" COPYONLY)
configure_file("C:/dev/VulkanSDK/1.2.170.0/Third-Party/Bin/SDL2.dll"
        "${PROJECT_SOURCE_DIR}/cmake-build-debug/" COPYONLY)

target_include_directories(Ray_Tracer_CPP PUBLIC
        "${PROJECT_BINARY_DIR}"
        "${PROJECT_SOURCE_DIR}"
        "C:/dev/VulkanSDK/1.2.170.0/Third-Party/Include/glm"
        "C:/dev/VulkanSDK/1.2.170.0/Third-Party/Include/SDL2"
        "C:/dev/glew/glew-2.1.0/include/GL")

#install(TARGETS Ray_Tracer_CPP CONFIGURATIONS Debug RUNTIME
#        DESTINATION "${PROJECT_SOURCE_DIR}/bin/Debug")
#install(TARGETS Ray_Tracer_CPP CONFIGURATIONS Release RUNTIME
#        DESTINATION "${PROJECT_SOURCE_DIR}/bin/Release")
