#pragma once

#include <iostream>
#include <mutex>
#include <thread>
#include <atomic>

#include "sdl_window/Renderer.h"
#include "Camera.h"
#include "Factory.h"
#include "objects/Bvh.h"

#define NUM_THREADS 15

class RayTracer
{
public:
    RayTracer(Camera& cam, Renderer& render_window) :
            cam(cam), render_window(render_window)
    {}

    void trace(const RenderingInfo& r_info);
    void stop();

private:
    glm::vec4 calculate_color(const Ray& r, RayPayload& rp, const int max_depth, RandomGenerator* random_generator);
    void calculate_pixel_rows(const RenderingInfo r_info);
    void calculate_pixel_rows_incremental(const RenderingInfo r_info);

    Camera& cam;
    Renderer& render_window;
    RandomGenerator rg;
    HitableList world;
    BvhNode bvh;
    std::mutex mutex;
    std::atomic<int> row;
    std::atomic<int> samples;
    std::thread threads[NUM_THREADS];
    bool threads_joined = false;
};
