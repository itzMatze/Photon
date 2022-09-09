#pragma once

#include <iostream>
#include <mutex>
#include <thread>
#include <atomic>

#include "sdl_window/Renderer.h"
#include "Camera.h"
#include "Factory.h"
#include "objects/Bvh.h"

class RayTracer
{
public:
    RayTracer(Renderer* render_window, uint32_t num_threads, RenderingInfo r_info) :
            render_window(render_window), num_threads(num_threads), r_info(r_info)
    {
        update_cam();
        load_scene();
    }

    void update_cam()
    { cam = Camera(r_info.origin, r_info.look_at, r_info.up, r_info.vfov, float(r_info.nx) / float(r_info.ny), r_info.aperture, r_info.focus_dist); }
    void update_render_window(Renderer* _render_window)
    { render_window = _render_window; }
    void update_render_info(RenderingInfo _r_info)
    { r_info = _r_info; }
    void load_scene();
    void trace();
    void stop();
    bool done();

private:
    glm::vec4 calculate_color(const Ray& r, RayPayload& rp, const int max_depth, RandomGenerator* random_generator);
    void calculate_pixel_rows(const RenderingInfo r_info, const Camera cam);
    void calculate_pixel_rows_incremental(const RenderingInfo r_info, const Camera cam);

    Camera cam;
    Renderer* render_window;
    RandomGenerator rg;
    HitableList world;
    BvhNode bvh;
    std::mutex mutex;
    std::atomic<int> row;
    std::atomic<int> samples;
    std::atomic<int> threads_done;
    std::vector<std::thread> threads;
    uint32_t num_threads;
    RenderingInfo r_info;
    bool threads_joined = false;
};
