#ifndef RAY_TRACING_IOW_HITRECORD_H
#define RAY_TRACING_IOW_HITRECORD_H

#include <memory>

class Material;

struct HitRecord
{
    float t;
    glm::vec3 p;
    glm::vec3 normal;
    std::shared_ptr<Material> mat;
};

#endif //RAY_TRACING_IOW_HITRECORD_H
