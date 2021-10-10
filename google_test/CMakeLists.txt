project(Google_tests)
set(google_test_dir X:/Stuff/Google_Test/googletest)
set(glm_dir X:/Stuff/VulkanSDK/1.2.176.1/Third-Party/Include/glm)
add_subdirectory(${google_test_dir} build)


include_directories(${google_test_dir}/googletest/include/ ${google_test_dir}/googletest/
        "${glm_dir}")

add_executable(Google_Tests_run ColorTest.cpp ../sdl_window/Color.cpp)

target_link_libraries(Google_Tests_run gtest gtest_main)
