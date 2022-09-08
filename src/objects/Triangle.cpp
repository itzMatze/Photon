#include "objects/Triangle.h"

#include <iostream>

bool Triangle::hit(const Ray &r, float t_min, float t_max, RayPayload& rp) const
{
    // Möller–Trumbore intersection algorithm
    constexpr float epsilon = 0.00001f;
    // find vectors for two edges sharing vert0 
    glm::vec3 edge1 = p1 - p0;
    glm::vec3 edge2 = p2 - p0;

    // begin calculating determinant - also used to calculate U parameter
    glm::vec3 pvec = glm::cross(r.direction(), edge2);

    // if determinant is near zero, ray lies in plane of triangle
    float det = glm::dot(edge1, pvec);
    float inv_det = 1.0f / det;

    // calculate distance from vert0 to ray origin
    glm::vec3 tvec = r.origin() - p0;

    glm::vec3 qvec = glm::cross(tvec, edge1);
  
    if (det > epsilon)
    {
        rp.uv.x = glm::dot(tvec, pvec);
        if (rp.uv.x < 0.0 || rp.uv.x > det) return false;
            
        // calculate V parameter and test bounds
        rp.uv.y = glm::dot(r.direction(), qvec);
        if (rp.uv.y < 0.0 || rp.uv.x + rp.uv.y > det) return false;  
    }
    else if(det < -epsilon)
    {
        // calculate U parameter and test bounds
        rp.uv.y = glm::dot(tvec, pvec);
        if (rp.uv.y > 0.0 || rp.uv.y < det) return false;
      
        // calculate V parameter and test bounds
        rp.uv.x = glm::dot(r.direction(), qvec) ;
        if (rp.uv.x > 0.0 || rp.uv.y + rp.uv.x < det) return false;
    }
    else return false;  // ray is parallel to the plane of the triangle

    rp.t = glm::dot(edge2, qvec) * inv_det;
    if (rp.t > t_max || rp.t < t_min) return false;
    rp.uv.y *= inv_det;
    rp.uv.x *= inv_det;
    rp.mat = mat.get();
    rp.normal = n;
    rp.p = r.point_at_parameter(rp.t);

    return true;
}

bool Triangle::bounding_box(Aabb& box) const
{
    box = Aabb(glm::min(p0, glm::min(p1, p2)) - glm::vec3(0.001f), glm::max(p0, glm::max(p1, p2)) + glm::vec3(0.001f));
    return true;
}
