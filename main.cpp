#include <iostream>
#include <mutex>
#include <thread>

#include "sdl_window/Renderer.h"
#include "Camera.h"
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

std::mutex mutex;
int sample_count[ny];

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

void calculate_pixel_rows(Camera* cam, Hitable* world, std::atomic<int>* row, std::atomic<int>* samples,
                          Renderer* render_window,
                          RandomGenerator* random_generator)
{
    // TODO: add opportunity to either render all samples directly or do it incrementally
    // TODO: check if the sample_count array can be replaced by using samples
    while (samples->load() > 0)
    {
        // as long as there are rows left, take one and calculate it
        for (int j = (*row)--; j >= 0; j = (*row)--)
        {
            // we could get in trouble if two threads are calculating the same row but for a different sample count
            // but this should really never happen (we need either very few rows or lots of threads)
            sample_count[j]++;
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
                color.values /= float(sample_count[j]);
                pixel_color.values *= (float(sample_count[j] - 1) / float(sample_count[j]));
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
            if (--(*samples) > 0)
            {
                *row = ny - 1;
                std::cout << "Remaining samples: " << *samples << std::endl;
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
    std::atomic<int> samples = ns;
    for (auto& t: threads)
    {
        t = std::thread(calculate_pixel_rows, cam, world, &row, &samples, render_window, random_generator);
    }
    // render loop
    bool quit = false;
    while (!quit)
    {
        render_window->render_frame();
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
                case SDL_QUIT:
                    quit = true;
                    // this line will trigger the thread joining block below
                    // the threads will finish their execution, because they check if row >= 0 and samples > 0
                    row = -NUM_THREADS - 1;
                    samples = 0;
                    break;
                case SDL_KEYDOWN:
                    switch (e.key.keysym.sym)
                    {
                        case SDLK_s:
                            if (threads_joined)
                            {
                                save_image((uint32_t*) render_window->get_pixels(), "", nx, ny, channels);
                                std::cout << "Image saved!" << std::endl;
                            }
                            else
                            {
                                std::cout << "Rendering is not finished. Saving failed!" << std::endl;
                            }
                            break;
                        case SDLK_1:
                            std::cout << "Scene 1" << std::endl;
                            row = -NUM_THREADS - 1;
                            samples = 0;
                            scene_index = 1;
                            break;
                        case SDLK_2:
                            std::cout << "Scene 2" << std::endl;
                            row = -NUM_THREADS - 1;
                            samples = 0;
                            scene_index = 2;
                            break;
                        default:
                            break;
                    }
                    break;
            }
        }
        // prevent threads from getting joined more than once
        // check if all threads are done with rendering, if there are no more rows left every thread will check this once and decrement row
        if (scene_index > 0 || samples < 1)
        {
            if (!threads_joined)
            {
                threads_joined = true;
                for (auto& t: threads)
                {
                    t.join();
                }
            }
            if (scene_index > 0)
            {
                // scene change or reload, restart rendering
                render_window->clean_surface(Color(0.0f, 0.0f, 0.0f, 0.0f));
                row = ny - 1;
                samples = ns;
                for (int& i: sample_count)
                {
                    i = 0;
                }
                switch (scene_index)
                {
                    case 1:
                        world = random_scene(random_generator);
                        break;
                    case 2:
                        world = create_scene();
                        break;
                    default:
                        std::cout << "Error: Default case in scene loading reached!" << std::endl;
                }
                scene_index = -1;
                for (auto& t: threads)
                {
                    t = std::thread(calculate_pixel_rows, cam, world, &row, &samples, render_window, random_generator);
                }
                threads_joined = false;
            }
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
    // TODO use imgui to build user interface where you are able to change the scene
    Hitable* scene = random_scene(&random_generator);

    for (int& i: sample_count)
    {
        i = 0;
    }

    trace(&cam, scene, &render_window, &random_generator);
    return 0;
}
