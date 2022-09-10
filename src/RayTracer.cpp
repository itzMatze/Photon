#include "RayTracer.h"

#include "Materials.h"

glm::vec4 RayTracer::calculate_color(const Ray& r, RayPayload& rp, const int max_depth, RandomGenerator* random_generator)
{
    if (rp.depth >= max_depth)
    {
        return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
    // intersection test
    if (bvh.hit(r, 0.001f, std::numeric_limits<float>::max(), rp))
    {
#if 1
        Ray scattered = {};
        glm::vec4 attenuation;
        // TODO attenuation should be calculated with phong when adding lights later on
        // TODO after adding lights and calculating the surface color with phong, add hard shadows
        // calculate a material dependent random scattered ray and trace it to get indirect lighting
        Color emitted = rp.mat->emitted(rp.uv, rp.p);
        if (rp.mat->scatter(r, rp, attenuation, random_generator, scattered))
        {
            rp.depth++;
            return emitted.values + attenuation * calculate_color(scattered, rp, max_depth, random_generator);
        }
        else
        {
            return emitted.values;
        }
#else
        // visualization of normals
        return 0.5f * glm::vec4(rp.normal.x + 1, rp.normal.y + 1, rp.normal.z + 1, 1.0f);
#endif
    }
    else
    {
        //float cos_dir_light = glm::dot(r.direction(), glm::normalize(glm::vec3(-0.1f, 1.0f, 0.1f)));
        //return cos_dir_light > 0.96f ? glm::vec4(20.0f, 20.0f, 20.0f, 1.0f) : glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
}

// render all samples at once, due to the missing synchronisation stuff this is a little faster
void RayTracer::calculate_pixel_rows(const RenderingInfo r_info, const Camera cam)
{
    RandomGenerator random_generator(true);
    Ray r;
    RayPayload rp;
    // as long as there are rows left, take one and calculate it
    for (int j = row--; j >= 0; j = row--)
    {
        // iterate over the pixels of the row
        for (int i = 0; i < r_info.nx; ++i)
        {
            Color color(0.0f, 0.0f, 0.0f);
            // number of samples
            for (int s = 0; s < r_info.ns; ++s)
            {
                rp.depth = 0;
                rp.wavelength = r_info.spectral ? static_cast<uint32_t>(random_generator.random_num() * 400.0f + 380.0f) : 580;
                float u = (float(i) + random_generator.random_num() - 0.5f) / float(r_info.nx);
                float v = (float(j) + random_generator.random_num() - 0.5f) / float(r_info.ny);
                r = cam.get_ray(u, v, &random_generator);
                // shoot ray
                color.values += r_info.spectral ? (Color::wavelength_to_rgba(rp.wavelength) * calculate_color(r, rp, r_info.max_depth, &random_generator)) : calculate_color(r, rp, r_info.max_depth, &random_generator);
            }
            color.values /= float(r_info.ns);
            renderer.set_pixel(i, r_info.ny - j - 1, color);
        }
    }
    threads_done++;
}

// render scene incrementally and accumulate samples, more synchronisation overhead and thus a little slower
// TODO: something is broke in incremental rendering. The result looks different from normal rendering.
//  Does not change if only using 1 thread, so no multithreading problem
void RayTracer::calculate_pixel_rows_incremental(const RenderingInfo r_info, const Camera cam)
{
    RandomGenerator random_generator(true);
    Ray r;
    RayPayload rp;
    // s is our sample count for the current row, if row gets reset there was no row left, so we take the sample count for the next iteration
    // this needs to be done here and inside the next loop, because not all threads get outside of the next loop, only one resets row
    for (int s = samples.load(); s <= r_info.ns; s = samples.load())
    {
        // as long as there are rows left, take one and calculate it
        for (int j = row--; j >= 0; j = row--)
        {
            // we get in trouble if two threads are calculating the same row but for a different sample count
            // and then overtakes the first thread, but this should really never happen (we need either very few rows or lots of threads)
            s = samples.load();
            Color color, pixel_color;
            // iterate through the pixels of the row
            for (int i = 0; i < r_info.nx; ++i)
            {
                rp.depth = 0;
                rp.wavelength = r_info.spectral ? static_cast<uint32_t>(random_generator.random_num() * 400.0f + 380.0f) : 580;
                // number of samples
                float u = (float(i) + random_generator.random_num() - 0.5f) / float(r_info.nx);
                float v = (float(j) + random_generator.random_num() - 0.5f) / float(r_info.ny);
                r = cam.get_ray(u, v, &random_generator);
                // shoot ray
                color.values = r_info.spectral ? (Color::wavelength_to_rgba(rp.wavelength) * calculate_color(r, rp, r_info.max_depth, &random_generator)) : calculate_color(r, rp, r_info.max_depth, &random_generator);
                // calculate relative weight of pixel_color and new calculated color sample
                pixel_color.values = renderer.get_pixel(i, renderer.render_height - j - 1);
                color.values = glm::mix(color.values, pixel_color.values, (double(s - 1) / double(s)));
                renderer.set_pixel(i, r_info.ny - j - 1, color);
            }
        }
        std::unique_lock<std::mutex> locker(mutex, std::defer_lock);
        locker.lock();
        // check if there is the need of resetting *row, no reset if another thread has already resetted *row
        if (row < 0)
        {
            // also, no reset if the sample_count is reached (threads get locked in the *row for loop if this doesn't get checked)
            if (++samples <= r_info.ns)
            {
                row = r_info.ny - 1;
                std::cout << "Remaining samples: " << r_info.ns - samples << std::endl;
            }
        }
        locker.unlock();
    }
    threads_done++;
}

void RayTracer::load_scene()
{
    switch (r_info.scene_index)
    {
        case 0:
            std::cout << "Restarting with last scene" << std::endl;
            break;
        case 1:
            world = random_scene(&rg);
            break;
        case 2:
            world = create_scene();
            break;
        case 3:
            world = line_scene();
            break;
        default:
            std::cout << "Error: Default case in scene loading reached!" << std::endl;
            break;
    }
    bvh = BvhNode(world.objects.begin(), world.objects.end());
}

void RayTracer::trace()
{
    threads_joined = false;
    renderer.clean_surface(Color(0.0f, 0.0f, 0.0f, 0.0f));
    // tells the threads which row to pick next for calculation
    row = renderer.render_height - 1;
    // and how many sample iterations are left
    samples = 1;
    threads_done = 0;
    renderer.set_use_surface(r_info.incremental);
    for (uint32_t t = 0; t < num_threads; ++t)
    {
        threads.push_back(std::thread((r_info.incremental ? &RayTracer::calculate_pixel_rows_incremental
                                            : &RayTracer::calculate_pixel_rows), this, r_info, cam));
    }
}

void RayTracer::stop()
{
    // the threads will finish their execution, because they check if row >= 0 and samples <= ns
    samples = r_info.ns;
    row = -1;
    // prevent joining threads more than one time
    if (!threads_joined)
    {
        threads_joined = true;
        for (auto& t : threads)
        {
            t.join();
        }
        threads.clear();
    }
}

bool RayTracer::done()
{
    return threads_done == num_threads;
}