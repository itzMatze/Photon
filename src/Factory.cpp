#include <iostream>
#include "Factory.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image_write.h"
#include "stb_image.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "objects/Segment.h"
#include "Materials.h"
#include "Lights.h"
#include "objects/Sphere.h"
#include "objects/Hitable.h"
#include "objects/Triangle.h"

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

void load_obj(std::string file, glm::mat4 transformation, std::shared_ptr<Material> mat, std::vector<std::shared_ptr<Hitable>>& objects)
{
    glm::mat3 norm_trans = glm::mat3(transformation);
    norm_trans = glm::transpose(glm::inverse(norm_trans));

    tinyobj::ObjReaderConfig reader_config;
    tinyobj::ObjReader reader;
    if (!reader.ParseFromFile(file, reader_config))
    {
        if (!reader.Error().empty())
        {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        return;
    }
    if (!reader.Warning().empty())
    {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    for (size_t s = 0; s < shapes.size(); s++)
    {
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); ++f)
        {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
            std::vector<glm::vec3> vertices;
            std::vector<glm::vec3> normals;
            for (size_t v = 0; v < fv; v++) {
                glm::vec4 vert;
                glm::vec3 norm;
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                vert.x = attrib.vertices[3*size_t(idx.vertex_index)+0];
                vert.y = attrib.vertices[3*size_t(idx.vertex_index)+1];
                vert.z = attrib.vertices[3*size_t(idx.vertex_index)+2];
                vert.w = 1.0f;

                vertices.push_back(transformation * vert);
                // Check if `normal_index` is zero or positive. negative = no normal data
                if (idx.normal_index >= 0) {
                    norm.x = attrib.normals[3*size_t(idx.normal_index)+0];
                    norm.y = attrib.normals[3*size_t(idx.normal_index)+1];
                    norm.z = attrib.normals[3*size_t(idx.normal_index)+2];
                }
                if (glm::length(norm) > 0.0f) normals.push_back(norm_trans * norm);
            }
            if (normals.size() == 3)
            {
                objects.push_back(std::make_shared<Triangle>(vertices[0], vertices[1], vertices[2], normals[0], normals[1], normals[2], mat));
            }
            else
            {
                objects.push_back(std::make_shared<Triangle>(vertices[0], vertices[1], vertices[2], mat));
            }
            index_offset += fv;
        }
    }
}

HitableList random_scene(RandomGenerator* random_generator)
{
    std::vector<std::shared_ptr<Hitable>> objects;
    std::shared_ptr<Material> squared_color = std::make_shared<Lambertian>(std::make_shared<CheckerTexture>(std::make_shared<ConstantTexture>(Color(0.9f, 0.9f, 0.9f)), std::make_shared<ConstantTexture>(Color(0.6f, 0.0f, 0.6f))));
    objects.emplace_back(std::make_shared<Sphere>(glm::vec3(0.0f, -10000.0f, 0.0f), 10000.0f, std::make_shared<DiffuseLight>(std::make_shared<NoiseTexture>(0.1f, true))));
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
                    std::shared_ptr<Material> color = std::make_shared<Dielectric>();
                    objects.push_back(std::make_shared<Sphere>(center, 0.2f, color));
                }
            }
        }
    }
    std::shared_ptr<Material> metal_color = std::make_shared<Metal>(glm::vec3(0.8f, 0.8f, 0.8f), 0.001f);
    objects.push_back(std::make_shared<Sphere>(glm::vec3(-1.0f, 2.0f, -2.6f), 1.0f, metal_color));
    int nx, ny, nn;
    unsigned char* pixels = stbi_load("../assets/textures/white.png", &nx, &ny, &nn, 0);
    objects.push_back(std::make_shared<Sphere>(glm::vec3(2.0f, 1.8f, -3.0f), 1.0f, std::make_shared<Lambertian>(std::make_shared<ImageTexture>(pixels, nx, ny, nn))));
    return HitableList(objects);
}

HitableList create_scene()
{
    glm::vec3 top(0.0f, 0.01f, -2.2f);
    glm::vec3 one(-0.75f, 1.5f, -2.2f);
    glm::vec3 two(0.0f, 1.5f, -2.95f);
    glm::vec3 three(0.75f, 1.5f, -2.2f);
    glm::vec3 four(0.0f, 1.5f, -1.45f);

    std::vector<std::shared_ptr<Hitable>> objects;
    // creating a few materials
    std::shared_ptr<Material> lambertian_1 = std::make_shared<Lambertian>(glm::vec3(0.5f, 0.1f, 0.7f));
    std::shared_ptr<Material> lambertian_2 = std::make_shared<Lambertian>(glm::vec3(1.0f, 1.0f, 1.0f));
    std::shared_ptr<Material> silver = std::make_shared<Metal>(glm::vec3(0.8f, 0.8f, 0.8f), 0.01f);
    std::shared_ptr<Material> gold = std::make_shared<Metal>(glm::vec3(0.8f, 0.6f, 0.2f), 0.1f);
    std::shared_ptr<Material> glass = std::make_shared<Dielectric>();
    // creating the spheres
    //objects.push_back(std::make_shared<Sphere>(glm::vec3(0.0f, 0.0f, -2.5f), 0.5f, lambertian_1));
    objects.push_back(std::make_shared<Triangle>(glm::vec3(50.0f, 0.0f, 50.0f), glm::vec3(50.0f, 0.0f, -50.0f), glm::vec3(-50.0f, 0.0f, 50.0f), lambertian_2));
    objects.push_back(std::make_shared<Triangle>(glm::vec3(-50.0f, 0.0f, 50.0f), glm::vec3(50.0f, 0.0f, -50.0f), glm::vec3(-50.0f, 0.0f, -50.0f), lambertian_2));
    objects.push_back(std::make_shared<Sphere>(glm::vec3(-1.7f, 0.35f, 0.0f), 0.3f, glass));
    //objects.push_back(std::make_shared<Sphere>(glm::vec3(-1.1f, 0.0f, -2.5f), 0.5f, gold));
    objects.push_back(std::make_shared<Sphere>(glm::vec3(-4.0f, 1.8f, -6.0f), 0.5f, std::make_shared<DiffuseLight>(glm::vec3(10.0f, 10.0f, 10.0f))));
    objects.push_back(std::make_shared<Sphere>(glm::vec3(5.0f, 2.5f, -10.0f), 0.9f, std::make_shared<DiffuseLight>(glm::vec3(8.0f, 4.0f, 8.0f))));
    objects.push_back(std::make_shared<Sphere>(glm::vec3(0.0f, 1.7f, -2.2f), 0.15f, std::make_shared<DiffuseLight>(glm::vec3(20.0f, 20.0f, 20.0f))));
    objects.push_back(std::make_shared<Sphere>(glm::vec3(3.0f, 1.0f, 0.0f), 0.5f, std::make_shared<DiffuseLight>(glm::vec3(11.0f, 11.0f, 11.0f))));
    objects.push_back(std::make_shared<Sphere>(glm::vec3(-3.0f, 1.0f, 0.0f), 0.5f, std::make_shared<DiffuseLight>(glm::vec3(11.0f, 11.0f, 11.0f))));
    objects.push_back(std::make_shared<Triangle>(top, one, two, glass));
    objects.push_back(std::make_shared<Triangle>(top, two, three, glass));
    objects.push_back(std::make_shared<Triangle>(top, three, four, glass));
    objects.push_back(std::make_shared<Triangle>(top, four, one, glass));
    objects.push_back(std::make_shared<Triangle>(one, four, two, glass));
    objects.push_back(std::make_shared<Triangle>(two, four, three, glass));

    top = glm::vec3(1.7f, 0.01f, -0.5f);
    one = glm::vec3(1.4f, 0.4f, -0.2f);
    two = glm::vec3(1.4f, 0.4f, -0.8f);
    three = glm::vec3(2.0f, 0.4f, -0.8f);
    four = glm::vec3(2.0f, 0.4f, -0.2f);
#if 0
    objects.push_back(std::make_shared<Triangle>(top, one, two, glass));
    objects.push_back(std::make_shared<Triangle>(top, two, three, glass));
    objects.push_back(std::make_shared<Triangle>(top, three, four, glass));
    objects.push_back(std::make_shared<Triangle>(top, four, one, glass));
    objects.push_back(std::make_shared<Triangle>(one, four, two, glass));
    objects.push_back(std::make_shared<Triangle>(two, four, three, glass));
#else
    objects.push_back(std::make_shared<Triangle>(one + glm::vec3(0.0f, -1.1f, 0.0f), one, two, glass));
    objects.push_back(std::make_shared<Triangle>(one + glm::vec3(0.0f, -1.1f, 0.0f), two, two + glm::vec3(0.0f, -1.1f, 0.0f), glass));
    objects.push_back(std::make_shared<Triangle>(two + glm::vec3(0.0f, -1.1f, 0.0f), two, three, glass));
    objects.push_back(std::make_shared<Triangle>(two + glm::vec3(0.0f, -1.1f, 0.0f), three, three + glm::vec3(0.0f, -1.1f, 0.0f), glass));
    objects.push_back(std::make_shared<Triangle>(three + glm::vec3(0.0f, -1.1f, 0.0f), three, four, glass));
    objects.push_back(std::make_shared<Triangle>(three + glm::vec3(0.0f, -1.1f, 0.0f), four, four + glm::vec3(0.0f, -1.1f, 0.0f), glass));
    objects.push_back(std::make_shared<Triangle>(four + glm::vec3(0.0f, -1.1f, 0.0f), four, one, glass));
    objects.push_back(std::make_shared<Triangle>(four + glm::vec3(0.0f, -1.1f, 0.0f), one, one + glm::vec3(0.0f, -1.1f, 0.0f), glass));
    objects.push_back(std::make_shared<Triangle>(four + glm::vec3(0.0f, -1.1f, 0.0f), one + glm::vec3(0.0f, -1.1f, 0.0f), three + glm::vec3(0.0f, -1.1f, 0.0f), glass));
    objects.push_back(std::make_shared<Triangle>(two + glm::vec3(0.0f, -1.1f, 0.0f), three + glm::vec3(0.0f, -1.1f, 0.0f), one + glm::vec3(0.0f, -1.1f, 0.0f), glass));
    objects.push_back(std::make_shared<Triangle>(two, one, three, glass));
    objects.push_back(std::make_shared<Triangle>(four, three, one, glass));
#endif
    top = glm::vec3(-2.3f, 1.0f, -5.0f);
    one = glm::vec3(-2.7f, 0.2f, -4.6f);
    two = glm::vec3(-2.7f, 0.2f, -5.4f);
    three = glm::vec3(-1.9f, 0.2f, -5.4f);
    four = glm::vec3(-1.9f, 0.2f, -4.6f);

    objects.push_back(std::make_shared<Triangle>(top, two, one, glass));
    objects.push_back(std::make_shared<Triangle>(top, three, two, glass));
    objects.push_back(std::make_shared<Triangle>(top, four, three, glass));
    objects.push_back(std::make_shared<Triangle>(top, one, four, glass));
    objects.push_back(std::make_shared<Triangle>(one, two, four, glass));
    objects.push_back(std::make_shared<Triangle>(two, three, four, glass));
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
    objects.push_back(std::make_shared<Sphere>(glm::vec3(6.8f, 1.3f, -2.2f), 5.6f, std::make_shared<DiffuseLight>(std::make_shared<ConstantTexture>(Color(4.0f, 4.0f, 4.0f)))));
    return HitableList(objects);
}