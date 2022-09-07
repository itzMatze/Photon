#include "objects/Segment.h"

float iSegment(glm::vec3 ro, glm::vec3 rd, glm::vec3 p0, glm::vec3 p1, glm::vec3& n)
{
    glm::vec3 ba = p1 - p0;
    glm::vec3 roa = ro - p0;

    float roard = dot(roa, rd);
    float rdba = dot(rd, ba);
    float baba = dot(ba, ba);
    float roaba = dot(roa, ba);

    glm::vec2 th = glm::vec2(-roard * baba + rdba * roaba, roaba - roard * rdba) / (baba - rdba * rdba);

    th.x = glm::max(th.x, 0.0f);
    th.y = glm::clamp(th.y, 0.0f, 1.0f);

    glm::vec3 p = p0 + ba * th.y;
    glm::vec3 q = ro + rd * th.x;
    n = p - q;

    return th.x;
}

float iSegment(glm::vec3 ro, glm::vec3 rd, glm::vec3 p0, glm::vec3 p1, float thickness, float t_max)
{
    // center the cylinder, normalize axis
    glm::vec3 cc = 0.5f * (p0 + p1);
    float ch = length(p1 - p0);
    glm::vec3 ca = (p1 - p0) / ch;
    ch *= 0.5;

    glm::vec3 oc = ro - cc;

    float card = dot(ca, rd);
    float caoc = dot(ca, oc);

    float a = 1.0f - card * card;
    float b = dot(oc, rd) - caoc * card;
    float c = dot(oc, oc) - caoc * caoc - (thickness) * (thickness);
    float h = b * b - a * c;
    if (h < 0.0) return t_max;
    h = sqrt(h);
    float t1 = (-b - h) / a;

    float y = caoc + t1 * card;

    // body
    if (abs(y) < ch) return t1;

    // caps
    float sy = y <= 0 ? (y == 0 ? 0.0f : -1.0f) : 1.0f;
    float tp = (sy * ch - caoc) / card;
    if (abs(b + a * tp) < h)
    {
        return tp;
    }
    return t_max;
}

bool Segment::hit(const Ray& ray, float t_min, float t_max, RayPayload& rp) const
{
    float t = iSegment(ray.origin(), ray.direction(), p0, p1, radius, t_max + 1.0f);
    if (t > t_min && t < t_max)
    {
        rp.p = ray.point_at_parameter(t);
        rp.normal = glm::cross(glm::cross((p1 - p0), (rp.p - p0)), (p1 - p0));
        rp.uv = glm::vec2(0.0f, 0.0f); // TODO: calculate correct texture coordinates
        rp.t = t;
        rp.mat = mat.get();
        return true;
    }
    return false;
}

bool Segment::bounding_box(Aabb& box) const
{
    glm::vec3 extent = (glm::vec3(radius)) * (glm::vec3(1.0f) - glm::normalize(p1 - p0));
    glm::vec3 min = glm::min(p0 - extent, p1 - extent);
    glm::vec3 max = glm::max(p0 + extent, p1 + extent);
    box = Aabb{min, max};
    return true;
}
