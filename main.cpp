#include <iostream>
#include <filesystem>
#include <random>
#include <thread>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "materials/Dielectric.h"
#include "materials/Metal.h"
#include "materials/Lambertian.h"
#include "objects/Sphere.h"
#include "objects/HitableList.h"
#include "Camera.h"
#include "sdl_window/Renderer.h"

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

std::uniform_real_distribution<float> distribution;
std::default_random_engine generator;

inline float random_num()
{
    return distribution(generator);
}

void save_image(uint32_t* pixels, const std::string& name)
{
    std::string filename;
    if (!name.empty())
    {
        filename = "_";
    }
    // getting time to add it to filename
#if 0
    time_t     now = time(nullptr);
    struct tm  tstruct;
    char       buf[80];
    localtime_s(&tstruct, &now);
    strftime(buf, sizeof(buf), " %Y-%m-%d_%H-%M-%S", &tstruct);
    std::string time(buf);
    filename.append(time);
#endif
    filename.append(name);
    filename.append(".png");
    // create target directory if needed
    std::filesystem::path images_path("../images/");
    if (!std::filesystem::exists(images_path))
    {
        std::filesystem::create_directory(images_path);
    }
    std::filesystem::path path_to_image;
    // if filename contains the time this index is useless, every file is unique by time (except you generate two files in one second)
    // be aware that this indexing uses empty indices in between e.g. if a file got deleted
    int index = 1;
    do
    {
        std::string index_string = std::to_string(index);
        path_to_image.clear();
        path_to_image = std::filesystem::path(images_path.string().append(index_string.append(filename)));
        index++;
    }
    while (std::filesystem::exists(path_to_image));
    stbi_write_png(path_to_image.string().c_str(), nx, ny, channels, pixels, nx * channels);
}

glm::vec3 random_in_unit_sphere()
{
    glm::vec3 p;
    do
    {
        p = 2.0f * glm::vec3(random_num(), random_num(), random_num()) - glm::vec3(1.0f, 1.0f, 1.0f);
    }
    while (glm::length(p) >= 1.0f);
    return p;
}

glm::vec4 calculate_color(const Ray& r, Hitable* world, int depth)
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
        if (depth < max_depth && rec.mat->scatter(r, rec, attenuation, scattered))
        {
            return attenuation * calculate_color(scattered, world, ++depth);
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

void calculate_pixel_rows(Camera* cam, Hitable* world, std::atomic<int>* row, Renderer* render_window)
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
                float u = (float(i) + random_num()) / float(nx);
                float v = (float(j) + random_num()) / float(ny);
                Ray r = cam->get_ray(u, v);
                // shoot ray
                color.values += calculate_color(r, world, 0);
                // keep the system responsive
                std::this_thread::yield();
            }
            color.values /= float(ns);
            color.values = glm::vec4(sqrt(color.values.r), sqrt(color.values.g), sqrt(color.values.b), 1.0f);
            render_window->set_pixel(i, ny - j - 1, color);
        }
    }
}

void trace(Camera* cam, Hitable* world, Renderer* render_window)
{
    std::thread threads[NUM_THREADS];
    bool threads_joined = false;
    // this variable tells the threads which row to pick next for calculation
    std::atomic<int> row = ny - 1;
    for (auto& t : threads)
    {
        t = std::thread(calculate_pixel_rows, cam, world, &row, render_window);
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

Hitable* random_scene()
{
    auto* list = new std::vector<Hitable*>;
    Lambertian* silver = new Lambertian(glm::vec3(0.2f, 0.5f, 0.5f));
    Sphere* s = new Sphere(glm::vec3(0.0f, -1000.0f, 0.0f), 1000.0f, *silver);
    list->push_back(s);
    for (int a = -11; a < 11; ++a)
    {
        for (int b = -11; b < 11; ++b)
        {
            float choose_mat = random_num();
            glm::vec3 center(a + 0.6f * random_num(), 0.2f, b + 0.6f * random_num());
            if (glm::length(center - glm::vec3(4.0f, 0.2f, 0.0f)) > 0.9f)
            {
                if (choose_mat < 0.7f)
                {
                    Lambertian* color = new Lambertian(
                            glm::vec3(glm::max(random_num(), 0.2f), glm::max(random_num(), 0.2f),
                                      glm::max(random_num(), 0.2f)));
                    s = new Sphere(center, 0.2f, *color);
                    list->push_back(s);
                }
                else if (choose_mat < 0.95f)
                {
                    Metal* color = new Metal(glm::vec3(glm::max(random_num(), 0.2f), glm::max(random_num(), 0.2f),
                                                       glm::max(random_num(), 0.2f)), 0.3f * random_num());
                    s = new Sphere(center, 0.2f, *color);
                    list->push_back(s);
                }
                else
                {
                    Dielectric* color = new Dielectric(1.5f);
                    s = new Sphere(center, 0.2f, *color);
                    list->push_back(s);
                }
            }
        }
    }
    Lambertian* color = new Lambertian(glm::vec3(0.1f, 0.8f, 0.9f));
    Metal* metal_color = new Metal(glm::vec3(0.8f, 0.8f, 0.8f), 0.001f);
    s = new Sphere(glm::vec3(-1.0f, 2.0f, -2.6f), 1.0f, *metal_color);
    list->push_back(s);
    s = new Sphere(glm::vec3(2.0f, 1.8f, -3.0f), 1.0f, *color);
    list->push_back(s);
    Hitable* ran_scene = new HitableList(list);
    return ran_scene;
}

int main()
{
    {
        // random number generator setup
        std::random_device rd;
        std::uniform_real_distribution<float> dis(0, 20000);
        generator = std::default_random_engine(dis(rd));
        distribution = std::uniform_real_distribution<float>(0, 1);
    }
    // creating scene, at the moment two scenes are getting generated but only one is used
    // TODO outsource scene creation stuff in a separate factory class
    std::vector<Hitable*> objects;
    Lambertian lambertian_1(glm::vec3(0.5f, 0.1f, 0.7f));
    Lambertian lambertian_2(glm::vec3(0.1f, 0.8f, 0.8f));
    Metal silver(glm::vec3(0.8f, 0.8f, 0.8f), 0.01f);
    Metal gold(glm::vec3(0.8f, 0.6f, 0.2f), 0.1f);
    Dielectric glass(1.5f);
    objects.push_back(new Sphere(glm::vec3(0.0f, 0.0f, -2.0f), 0.5, lambertian_1));
    objects.push_back(new Sphere(glm::vec3(0.0f, -100.5f, -2.0f), 100, lambertian_2));
    objects.push_back(new Sphere(glm::vec3(1.1f, 0.0f, -2.0f), 0.5f, glass));
    objects.push_back(new Sphere(glm::vec3(-1.1f, 0.0f, -2.0f), 0.5f, gold));
    objects.push_back(new Sphere(glm::vec3(0.3f, -0.3f, -1.1f), 0.2f, silver));
    Hitable* world = new HitableList(&objects);
    Hitable* ran_scene = random_scene();
    // TODO if aperture is set to 0, skip corresponding calculations
    Camera cam(glm::vec3(0.0f, 1.5f, 0.0f), glm::vec3(0.0f, 0.0f, -15.0f), glm::vec3(0.0f, 1.0f, 0.0f),
               90.0f, float(nx) / float(ny), 0.01f, 2.0f);
    Renderer render_window(1000, nx, ny);
    // TODO trace image incrementally, first one sample per pixel and then accumulate more
    // TODO probably also render first in lower resolution and then add resolution (this is probably not so easy)
    trace(&cam, ran_scene, &render_window);
    save_image((uint32_t*) render_window.get_pixels(), "");
    return 0;
}
