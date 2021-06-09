#include "Sphere.h"

bool Sphere::hit(const Ray &r, float t_min, float t_max, HitRecord& rec) const
{
    glm::vec3 oc = r.origin - center;
    float a = glm::dot(r.direction, r.direction);
    float b = 2.0f * glm::dot(oc, r.direction);
    float c = glm::dot(oc, oc) - radius * radius;
    float discriminant = b * b - 4 * a * c;
    if (discriminant > 0)
    {
        float temp = (-b - sqrt(b * b - 4 * a * c)) / (2.0f * a);
        if (temp < t_max && temp > t_min)
        {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.mat = &mat;
            return true;
        }
        temp = (-b + sqrt(b * b - 4 * a * c)) / (2.0f * a);
        if (temp < t_max && temp > t_min)
        {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.mat = &mat;
            return true;
        }
    }
    return false;
}
