#include "RayTracer.h"

glm::vec4 RayTracer::calculate_color(const Ray& r, int depth, const int max_depth)
{
    HitRecord rec;
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
        if (depth < max_depth && rec.mat->scatter(r, rec, attenuation, &random_generator, scattered))
        {
            return attenuation * calculate_color(scattered, ++depth, max_depth);
        }
        else
        {
            return glm::vec4{0.0f, 0.0f, 0.0f, 1.0f};
        }
#else
        // visualization of normals
        return 0.5f * glm::vec4(rec.normal.x + 1, rec.normal.y + 1, rec.normal.z + 1, 1.0f);
#endif
    }
    else
    {
        // if the ray didn't hit anything, paint background, this is also currently the 'light source'
        glm::vec3 unit_direction = glm::normalize(r.direction());
        float t = 0.5f * (unit_direction.y + 1.0f);
        return (1.0f - t) * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) + t * glm::vec4(0.5f, 0.7f, 1.0f, 1.0f);
    }
}

// render all samples at once, due to the missing synchronisation stuff this is a little faster
void RayTracer::calculate_pixel_rows(const int ns, const int max_depth)
{
    // as long as there are rows left, take one and calculate it
    for (int j = row--; j >= 0; j = row--)
    {
        // iterate through the pixels of the row
        for (int i = 0; i < render_window->render_width; ++i)
        {
            Color color(0.0f, 0.0f, 0.0f);
            // number of samples
            for (int s = 0; s < ns; ++s)
            {
                float u = (float(i) + random_generator.random_num()) / float(render_window->render_width);
                float v = (float(j) + random_generator.random_num()) / float(render_window->render_height);
                Ray r = cam->get_ray(u, v, &random_generator);
                // shoot ray
                color.values += calculate_color(r, 0, max_depth);
                // keep the system responsive
                std::this_thread::yield();
            }
            color.values /= float(ns);
            render_window->set_pixel(i, render_window->render_height - j - 1, color);
        }
    }
    samples = ns + 1;
}

// render scene incrementally and accumulate samples, more synchronisation overhead and thus a little slower
void RayTracer::calculate_pixel_rows_incremental(const int ns, const int max_depth)
{
    // s is our sample count for the current row, if row gets reset there was no row left, so we take the sample count for the next iteration
    // this needs to be done here and inside the next loop, because not all threads get outside of the next loop, only one resets row
    for (int s = samples.load(); s <= ns; s = samples.load())
    {
        // as long as there are rows left, take one and calculate it
        for (int j = row--; j >= 0; j = row--)
        {
            // we get in trouble if two threads are calculating the same row but for a different sample count
            // and then overtakes the first thread, but this should really never happen (we need either very few rows or lots of threads)
            s = samples.load();
            // iterate through the pixels of the row
            for (int i = 0; i < render_window->render_width; ++i)
            {
                Color color;
                Color pixel_color = render_window->get_pixel(i, render_window->render_height - j - 1);
                // number of samples
                float u = (float(i) + random_generator.random_num()) / float(render_window->render_width);
                float v = (float(j) + random_generator.random_num()) / float(render_window->render_height);
                Ray r = cam->get_ray(u, v, &random_generator);
                // shoot ray
                color.values += calculate_color(r, 0, max_depth);
                // keep the system responsive
                std::this_thread::yield();
                // calculate relative weight of pixel_color and new calculated color sample
                color.values /= float(s);
                pixel_color.values *= (float(s - 1) / float(s));
                color.values += pixel_color.values;
                render_window->set_pixel(i, render_window->render_height - j - 1, color);
            }
        }
        std::unique_lock<std::mutex> locker(mutex, std::defer_lock);
        locker.lock();
        // check if there is the need of resetting *row, no reset if another thread has already resetted *row
        if (row < 0)
        {
            // also, no reset if the sample_count is reached (threads get locked in the *row for loop if this doesn't get checked)
            if (++samples <= ns)
            {
                row = render_window->render_height - 1;
                std::cout << "Remaining samples: " << ns - samples << std::endl;
            }
        }
        locker.unlock();
    }
}

void RayTracer::trace(const RenderingInfo& r_info)
{
    switch (r_info.scene_index)
    {
        case 0:
            std::cout << "Restarting with last scene" << std::endl;
            break;
        case 1:
            world = random_scene(&random_generator);
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
    threads_joined = false;
    render_window->clean_surface(Color(0.0f, 0.0f, 0.0f, 0.0f));
    // tells the threads which row to pick next for calculation
    row = render_window->render_height - 1;
    // and how many sample iterations are left
    samples = 1;
    for (auto& t: threads)
    {
        t = std::thread((r_info.incremental ? &RayTracer::calculate_pixel_rows_incremental
                                            : &RayTracer::calculate_pixel_rows), this, r_info.ns, r_info.max_depth);
    }
}

void RayTracer::stop()
{
    // the threads will finish their execution, because they check if row >= 0 and samples <= ns
    row = -NUM_THREADS - 1;
    samples = std::numeric_limits<int>::max() - NUM_THREADS;
    // prevent joining threads more than one time
    if (!threads_joined)
    {
        threads_joined = true;
        for (auto& t: threads)
        {
            t.join();
        }
    }
}
