#include <iostream>
#include <thread>

#include "Camera.h"
#include "sdl_window/Renderer.h"
#include "Factory.h"

#define NUM_THREADS 14
// choose to render image fast or in great quality
#if 1
constexpr int nx = 1000;
constexpr int ny = 800;
constexpr int ns = 4;
constexpr int max_depth = 6;
#else
constexpr int nx = 3840;
constexpr int ny = 2160;
constexpr int ns = 256;
constexpr int max_depth = 20;
#endif
constexpr int channels = 4;

glm::vec4 calculate_color(const Ray& r, Hitable* world, RandomGenerator* random_generator, int depth)
{
    HitRecord rec = {};
    // intersection test
    if (world->hit(r, 0.001f, std::numeric_limits<float>::max(), rec))
    {
#if 1
        Ray scattered = {};
        glm::vec4 attenuation;
        // TODO attenuation should be calculated with phong when adding lights later on
        // TODO after adding lights and calculating the surface color with phong, add hard shadows
        // TODO probably add ability to send more than one scattered rays
        // calculate a material dependent random scattered ray and trace it to get indirect lighting
        if (depth < max_depth && rec.mat->scatter(r, rec, attenuation, random_generator, scattered))
        {
            return attenuation * calculate_color(scattered, world, random_generator, ++depth);
        }
        else
        {
            return glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        }
#else
        // visualization of normals
        return 0.5f * glm::vec4(rec.normal.x + 1, rec.normal.y + 1, rec.normal.z + 1, 1.0f);
#endif
    }
    else
    {
        // if the ray didn't hit anything, paint background
        glm::vec3 unit_direction = glm::normalize(r.direction);
        float t = 0.5f * (unit_direction.y + 1.0f);
        return (1.0f - t) * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) + t * glm::vec4(0.5f, 0.7f, 1.0f, 1.0f);
    }
}

void calculate_pixel_rows(Camera* cam, Hitable* world, std::atomic<int>* row, Renderer* render_window, RandomGenerator* random_generator)
{
    // as long as there are rows left, take one and calculate it
    for (int j = (*row)--; j >= 0; j = (*row)--)
    {
        // iterate through the pixels of the row
        for (int i = 0; i < nx; ++i)
        {
            Color color(0.0f, 0.0f, 0.0f);
            // number of samples
            for (int s = 0; s < ns; ++s)
            {
                float u = (float(i) + random_generator->random_num()) / float(nx);
                float v = (float(j) + random_generator->random_num()) / float(ny);
                Ray r = cam->get_ray(u, v, random_generator);
                // shoot ray
                color.values += calculate_color(r, world, random_generator, 0);
                // keep the system responsive
                std::this_thread::yield();
            }
            color.values /= float(ns);
            color.values = glm::vec4(sqrt(color.values.r), sqrt(color.values.g), sqrt(color.values.b), 1.0f);
            render_window->set_pixel(i, ny - j - 1, color);
        }
    }
}

void trace(Camera* cam, Hitable* world, Renderer* render_window, RandomGenerator* random_generator)
{
    std::thread threads[NUM_THREADS];
    bool threads_joined = false;
    // this variable tells the threads which row to pick next for calculation
    std::atomic<int> row = ny - 1;
    for (auto& t : threads)
    {
        t = std::thread(calculate_pixel_rows, cam, world, &row, render_window, random_generator);
    }
    // render loop
    bool quit = false;
    while (!quit)
    {
        render_window->render_frame();
        SDL_Event e;
        if (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
                // this line will trigger the thread joining block below
                // the threads will immediately finish their execution, because they check if row >= 0
                row = -NUM_THREADS - 1;
            }
        }
        // prevent threads from getting joined more than once
        // check if all threads are done with rendering, if there are no more rows left every thread will check this once and decrement row
        if (!threads_joined && row < -NUM_THREADS)
        {
            threads_joined = true;
            for (auto& t : threads)
            {
                t.join();
            }
        }
    }
}

int main()
{
    Camera cam(glm::vec3(0.0f, 1.5f, 0.0f), glm::vec3(0.0f, 0.0f, -15.0f), glm::vec3(0.0f, 1.0f, 0.0f),
               90.0f, float(nx) / float(ny), 0.01f, 2.0f);
    Renderer render_window(1000, nx, ny);
    // TODO trace image incrementally, first one sample per pixel and then accumulate more
    // TODO probably also render first in lower resolution and then add resolution (this is probably not so easy)
    RandomGenerator random_generator;
    Hitable* scene = random_scene(&random_generator);
    trace(&cam, scene, &render_window, &random_generator);
    save_image((uint32_t*) render_window.get_pixels(), "", nx, ny, channels);
    return 0;
}
