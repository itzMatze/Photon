#include "objects/Sphere.h"

bool Sphere::hit(const Ray &r, float t_min, float t_max, HitRecord& rec) const
{
    glm::vec3 oc = r.origin() - center;
    float a = glm::dot(r.direction(), r.direction());
    float b = 2.0f * glm::dot(oc, r.direction());
    float c = glm::dot(oc, oc) - radius * radius;
    float discriminant = b * b - 4 * a * c;
    if (discriminant > 0)
    {
        float temp = (-b - sqrt(b * b - 4 * a * c)) / (2.0f * a);
        if (temp < t_max && temp > t_min)
        {
            rec.p = r.point_at_parameter(temp);
            rec.normal = (rec.p - center) / radius;
            rec.uv = get_uv((rec.p - center) / radius);
            rec.t = temp;
            rec.mat = mat.get();
            return true;
        }
        temp = (-b + sqrt(b * b - 4 * a * c)) / (2.0f * a);
        if (temp < t_max && temp > t_min)
        {
            rec.p = r.point_at_parameter(temp);
            rec.normal = (rec.p - center) / radius;
            rec.uv = get_uv((rec.p - center) / radius);
            rec.t = temp;
            rec.mat = mat.get();
            return true;
        }
    }
    return false;
}

bool Sphere::bounding_box(Aabb& box) const
{
    box = Aabb(center - glm::vec3(radius, radius, radius), center + glm::vec3(radius, radius, radius));
    return true;
}

glm::vec2 Sphere::get_uv(const glm::vec3& p) const
{
    float phi = atan2(p.z, p.x);
    float theta = asin(p.y);
    glm::vec2 uv = glm::vec2(1 - (phi + M_PI) / (2 * M_PI), (theta + M_PI / 2) / M_PI);
    return uv;
}
