#include "Factory.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

void save_image(uint32_t* pixels, const std::string& name, int nx, int ny, int channels)
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

Hitable* random_scene(RandomGenerator* random_generator)
{
    auto* list = new std::vector<Hitable*>;
    Lambertian* silver = new Lambertian(glm::vec3(0.2f, 0.5f, 0.5f));
    Sphere* s = new Sphere(glm::vec3(0.0f, -1000.0f, 0.0f), 1000.0f, *silver);
    list->push_back(s);
    for (int a = -11; a < 11; ++a)
    {
        for (int b = -11; b < 11; ++b)
        {
            float choose_mat = random_generator->random_num();
            glm::vec3 center(a + 0.6f * random_generator->random_num(), 0.2f, b + 0.6f * random_generator->random_num());
            if (glm::length(center - glm::vec3(4.0f, 0.2f, 0.0f)) > 0.9f)
            {
                if (choose_mat < 0.7f)
                {
                    Lambertian* color = new Lambertian(
                            glm::vec3(glm::max(random_generator->random_num(), 0.2f), glm::max(random_generator->random_num(), 0.2f),
                                      glm::max(random_generator->random_num(), 0.2f)));
                    s = new Sphere(center, 0.2f, *color);
                    list->push_back(s);
                }
                else if (choose_mat < 0.95f)
                {
                    Metal* color = new Metal(glm::vec3(glm::max(random_generator->random_num(), 0.2f), glm::max(random_generator->random_num(), 0.2f),
                                                       glm::max(random_generator->random_num(), 0.2f)), 0.3f * random_generator->random_num());
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

Hitable* create_scene()
{
    auto* objects = new std::vector<Hitable*>;
    // creating a few materials
    Lambertian* lambertian_1 = new Lambertian(glm::vec3(0.5f, 0.1f, 0.7f));
    Lambertian* lambertian_2 = new Lambertian(glm::vec3(0.1f, 0.8f, 0.8f));
    Metal* silver = new Metal(glm::vec3(0.8f, 0.8f, 0.8f), 0.01f);
    Metal* gold = new Metal(glm::vec3(0.8f, 0.6f, 0.2f), 0.1f);
    Dielectric* glass = new Dielectric(1.5f);
    // creating the spheres
    objects->push_back(new Sphere(glm::vec3(0.0f, 0.0f, -2.0f), 0.5, *lambertian_1));
    objects->push_back(new Sphere(glm::vec3(0.0f, -100.5f, -2.0f), 100, *lambertian_2));
    objects->push_back(new Sphere(glm::vec3(1.1f, 0.0f, -2.0f), 0.5f, *glass));
    objects->push_back(new Sphere(glm::vec3(-1.1f, 0.0f, -2.0f), 0.5f, *gold));
    objects->push_back(new Sphere(glm::vec3(0.3f, -0.3f, -1.1f), 0.2f, *silver));
    Hitable* ran_scene = new HitableList(objects);
    return ran_scene;
}
