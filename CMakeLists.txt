cmake_minimum_required(VERSION 3.19)
project(Ray_Tracer_CPP)

set(CMAKE_CXX_STANDARD 20)

add_executable(Ray_Tracer_CPP main.cpp)

target_include_directories(Ray_Tracer_CPP PUBLIC
        "${PROJECT_BINARY_DIR}"
        "${PROJECT_SOURCE_DIR}")

install(TARGETS Ray_Tracer_CPP CONFIGURATIONS Debug RUNTIME
        DESTINATION "${PROJECT_SOURCE_DIR}/bin/Debug")
install(TARGETS Ray_Tracer_CPP CONFIGURATIONS Release RUNTIME
        DESTINATION "${PROJECT_SOURCE_DIR}/bin/Release")
