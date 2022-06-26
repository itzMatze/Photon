#include "Factory.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "objects/Segment.h"

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

HitableList random_scene(RandomGenerator* random_generator)
{
    std::vector<std::shared_ptr<Hitable>> objects;
    std::shared_ptr<Material> squared_color = std::make_shared<Lambertian>(std::make_shared<CheckerTexture>(std::make_shared<ConstantTexture>(Color(0.9f, 0.9f, 0.9f)), std::make_shared<ConstantTexture>(Color(0.6f, 0.0f, 0.6f))));
    objects.emplace_back(std::make_shared<Sphere>(glm::vec3(0.0f, -10000.0f, 0.0f), 10000.0f, std::make_shared<Lambertian>(std::make_shared<NoiseTexture>(0.1f, true))));
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
                    std::shared_ptr<Material> color = std::make_shared<Lambertian>(
                            glm::vec3(glm::max(random_generator->random_num(), 0.2f), glm::max(random_generator->random_num(), 0.2f),
                                      glm::max(random_generator->random_num(), 0.2f)));
                    objects.push_back(std::make_shared<Sphere>(center, 0.2f, color));
                }
                else if (choose_mat < 0.95f)
                {
                    std::shared_ptr<Material> color = std::make_shared<Metal>(glm::vec3(glm::max(random_generator->random_num(), 0.2f), glm::max(random_generator->random_num(), 0.2f),
                                                       glm::max(random_generator->random_num(), 0.2f)), 0.3f * random_generator->random_num());
                    objects.push_back(std::make_shared<Sphere>(center, 0.2f, color));
                }
                else
                {
                    std::shared_ptr<Material> color = std::make_shared<Dielectric>(1.5f);
                    objects.push_back(std::make_shared<Sphere>(center, 0.2f, color));
                }
            }
        }
    }
    std::shared_ptr<Material> metal_color = std::make_shared<Metal>(glm::vec3(0.8f, 0.8f, 0.8f), 0.001f);
    objects.push_back(std::make_shared<Sphere>(glm::vec3(-1.0f, 2.0f, -2.6f), 1.0f, metal_color));
    objects.push_back(std::make_shared<Sphere>(glm::vec3(2.0f, 1.8f, -3.0f), 1.0f, std::make_shared<Lambertian>(std::make_shared<NoiseTexture>(15.1f, true))));
    return HitableList(objects);
}

HitableList create_scene()
{
    std::vector<std::shared_ptr<Hitable>> objects;
    // creating a few materials
    std::shared_ptr<Material> lambertian_1 = std::make_shared<Lambertian>(glm::vec3(0.5f, 0.1f, 0.7f));
    std::shared_ptr<Material> lambertian_2 = std::make_shared<Lambertian>(glm::vec3(0.1f, 0.8f, 0.8f));
    std::shared_ptr<Material> silver = std::make_shared<Metal>(glm::vec3(0.8f, 0.8f, 0.8f), 0.01f);
    std::shared_ptr<Material> gold = std::make_shared<Metal>(glm::vec3(0.8f, 0.6f, 0.2f), 0.1f);
    std::shared_ptr<Material> glass = std::make_shared<Dielectric>(1.5f);
    // creating the spheres
    objects.push_back(std::make_shared<Sphere>(glm::vec3(0.0f, 0.0f, -2.0f), 0.5f, lambertian_1));
    objects.push_back(std::make_shared<Sphere>(glm::vec3(0.0f, -100.5f, -2.0f), 100.0f, lambertian_2));
    objects.push_back(std::make_shared<Sphere>(glm::vec3(1.1f, 0.0f, -2.0f), 0.5f, glass));
    objects.push_back(std::make_shared<Sphere>(glm::vec3(-1.1f, 0.0f, -2.0f), 0.5f, gold));
    objects.push_back(std::make_shared<Sphere>(glm::vec3(0.3f, -0.3f, -1.1f), 0.2f, silver));
    return HitableList(objects);
}

HitableList line_scene()
{
    std::vector<std::shared_ptr<Hitable>> objects;
    std::shared_ptr<Material> mat = std::make_shared<Lambertian>(glm::vec3(0.1f, 0.8f, 0.8f));
    std::shared_ptr<Material> silver = std::make_shared<Metal>(glm::vec3(0.8f, 0.8f, 0.8f), 0.01f);
    std::shared_ptr<Material> gold = std::make_shared<Metal>(glm::vec3(0.8f, 0.6f, 0.2f), 0.1f);
    glm::vec3 p0, p1;
    p0 = glm::vec3(1.0f, 3.0f, -3.0f);
    p1 = glm::vec3(-1.0f, -1.0f, -4.0f);
    objects.push_back(std::make_shared<Segment>(p0, p1, 0.2f, silver));
    p0 = glm::vec3(1.5f, 3.0f, -2.0f);
    p1 = glm::vec3(-0.5f, -1.0f, -3.0f);
    objects.push_back(std::make_shared<Segment>(p0, p1, 0.2f, gold));
    objects.push_back(std::make_shared<Sphere>(glm::vec3(-1.3f, 1.3f, -2.1f), 0.6f, mat));
    return HitableList(objects);
}