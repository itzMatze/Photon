#include <iostream>
#include <mutex>
#include <thread>

#include "sdl_window/Renderer.h"
#include "Camera.h"
#include "Factory.h"

#define NUM_THREADS 15
// choose to render image fast or in great quality
#if 1
constexpr int nx = 1000;
constexpr int ny = 800;
constexpr int ns = 16;
constexpr int max_depth = 10;
#else
constexpr int nx = 3840;
constexpr int ny = 2160;
constexpr int ns = 256;
constexpr int max_depth = 15;
#endif
constexpr int channels = 4;

std::mutex mutex;

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
        // if the ray didn't hit anything, paint background, this is also currently the 'light source'
        glm::vec3 unit_direction = glm::normalize(r.direction);
        float t = 0.5f * (unit_direction.y + 1.0f);
        return (1.0f - t) * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) + t * glm::vec4(0.5f, 0.7f, 1.0f, 1.0f);
    }
}

// render all samples at once, due to the missing synchronisation stuff this is a little faster
void calculate_pixel_rows(Camera* cam, Hitable* world, std::atomic<int>* row, std::atomic<int>* samples, Renderer* render_window,
                          RandomGenerator* random_generator)
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
            render_window->set_pixel(i, ny - j - 1, color);
        }
    }
    *samples = ns + 1;
}

// render scene incrementally and accumulate samples, more synchronisation overhead and thus a little slower
void calculate_pixel_rows_incremental(Camera* cam, Hitable* world, std::atomic<int>* row, std::atomic<int>* samples,
                          Renderer* render_window,
                          RandomGenerator* random_generator)
{
    // s is our sample count for the current row, if row gets reset there was no row left, so we take the sample count for the next iteration
    // this needs to be done here and inside the next loop, because not all threads get outside of the next loop, only one resets row
    for (int s = samples->load(); s <= ns; s = samples->load())
    {
        // as long as there are rows left, take one and calculate it
        for (int j = (*row)--; j >= 0; j = (*row)--)
        {
            // we get in trouble if two threads are calculating the same row but for a different sample count
            // and then overtakes the first thread, but this should really never happen (we need either very few rows or lots of threads)
            s = samples->load();
            // iterate through the pixels of the row
            for (int i = 0; i < nx; ++i)
            {
                Color color;
                Color pixel_color = render_window->get_pixel(i, ny - j - 1);
                // number of samples
                float u = (float(i) + random_generator->random_num()) / float(nx);
                float v = (float(j) + random_generator->random_num()) / float(ny);
                Ray r = cam->get_ray(u, v, random_generator);
                // shoot ray
                color.values += calculate_color(r, world, random_generator, 0);
                // keep the system responsive
                std::this_thread::yield();
                // calculate relative weight of pixel_color and new calculated color sample
                color.values /= float(s);
                pixel_color.values *= (float(s - 1) / float(s));
                color.values += pixel_color.values;
                render_window->set_pixel(i, ny - j - 1, color);
            }
        }
        std::unique_lock<std::mutex> locker(mutex, std::defer_lock);
        locker.lock();
        // check if there is the need of resetting *row, no reset if another thread has already resetted *row
        if (*row < 0)
        {
            // also, no reset if the sample_count is reached (threads get locked in the *row for loop if this doesn't get checked)
            if (++(*samples) <= ns)
            {
                *row = ny - 1;
                std::cout << "Remaining samples: " << ns - *samples << std::endl;
            }
        }
        locker.unlock();
    }
}

void trace(Camera* cam, Hitable* world, Renderer* render_window, RandomGenerator* random_generator)
{
    std::thread threads[NUM_THREADS];
    bool threads_joined = false;
    int scene_index = -1;
    // tells the threads which row to pick next for calculation
    std::atomic<int> row = ny - 1;
    // and how many sample iterations are left
    std::atomic<int> samples = 1;
    for (auto& t: threads)
    {
        t = std::thread(calculate_pixel_rows_incremental, cam, world, &row, &samples, render_window, random_generator);
    }
    // render loop
    bool quit = false;
    bool incremental = true;
    bool save = false;
    bool show_gui = true;
    while (!quit)
    {
        render_window->render_frame(show_gui, scene_index, incremental, save);
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            ImGui_ImplSDL2_ProcessEvent(&e);
            if (e.window.event == SDL_WINDOWEVENT_CLOSE)
            {
                quit = true;
                // this line will trigger the thread joining block below
                // the threads will finish their execution, because they check if row >= 0 and samples <= ns
                row = -NUM_THREADS - 1;
                samples = ns + 1;
            }
            switch (e.type)
            {
                case SDL_KEYDOWN:
                    switch (e.key.keysym.sym)
                    {

                        case SDLK_g:
                            show_gui = !show_gui;
                            break;
                        default:
                            break;
                    }
                    break;
            }
        }
        // prevent threads from getting joined more than once
        // check if all threads are done with rendering, if there are no more rows left every thread will check this once and decrement row
        if (scene_index >= 0 || samples > ns)
        {
            row = -NUM_THREADS - 1;
            samples = ns + 1;
            if (!threads_joined)
            {
                threads_joined = true;
                for (auto& t: threads)
                {
                    t.join();
                }
            }
            if (scene_index >= 0)
            {
                // scene change or reload, restart rendering
                render_window->clean_surface(Color(0.0f, 0.0f, 0.0f, 0.0f));
                row = ny - 1;
                samples = 1;
                switch (scene_index)
                {
                    case 0:
                        std::cout << "Restarting with last scene" << std::endl;
                        break;
                    case 1:
                        world = random_scene(random_generator);
                        break;
                    case 2:
                        world = create_scene();
                        break;
                    case 3:
                        world = bezier_scene();
                        break;
                    default:
                        std::cout << "Error: Default case in scene loading reached!" << std::endl;
                }
                scene_index = -1;
                for (auto& t: threads)
                {
                    t = incremental ?
                            std::thread(calculate_pixel_rows_incremental, cam, world, &row, &samples, render_window, random_generator)
                            : std::thread(calculate_pixel_rows, cam, world, &row, &samples, render_window, random_generator);
                }
                threads_joined = false;
            }
        }
        if (save)
        {
            if (threads_joined)
            {
                save_image((uint32_t*) render_window->get_pixels(), "", nx, ny, channels);
                std::cout << "Image saved!" << std::endl;
            }
            else
            {
                std::cout << "Rendering is not finished. Saving failed!" << std::endl;
            }
            save = false;
        }
        std::this_thread::yield();
    }
}

int main()
{
    Camera cam(glm::vec3(0.0f, 1.5f, 0.0f), glm::vec3(0.0f, 0.0f, -15.0f), glm::vec3(0.0f, 1.0f, 0.0f),
               90.0f, float(nx) / float(ny), 0.01f, 2.0f);
    Renderer render_window(1000, nx, ny);
    // TODO probably also render first in lower resolution and then add resolution (this is probably not so easy)
    RandomGenerator random_generator;
    Hitable* scene = random_scene(&random_generator);

    trace(&cam, scene, &render_window, &random_generator);
    return 0;
}
