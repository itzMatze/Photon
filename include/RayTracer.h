#pragma once

#include <iostream>
#include <mutex>
#include <thread>
#include <atomic>

#include "sdl_window/Renderer.h"
#include "Camera.h"
#include "Factory.h"

#define NUM_THREADS 15

class RayTracer
{
public:
    RayTracer(Camera* cam, Renderer* render_window) :
            cam(cam), render_window(render_window)
    {}

    void trace(const RenderingInfo& r_info);
    void stop();

private:
    glm::vec4 calculate_color(const Ray& r, int depth, const int max_depth);
    void calculate_pixel_rows(const int ns, const int max_depth);
    void calculate_pixel_rows_incremental(const int ns, const int max_depth);

    Camera* cam;
    Renderer* render_window;
    RandomGenerator random_generator;
    std::shared_ptr<Hitable> world;
    std::mutex mutex;
    std::atomic<int> row;
    std::atomic<int> samples;
    std::thread threads[NUM_THREADS];
    bool threads_joined = false;
};
