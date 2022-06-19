#include "objects/Aabb.h"
#include "Ray.h"

bool Aabb::hit(const Ray& r, float tmin, float tmax) const
{
    for (int i = 0; i < 3; ++i)
    {
        float invD = 1.0f / r.direction()[i];
        float t0 = (min()[i] - r.origin()[i]) * invD;
        float t1 = (max()[i] - r.origin()[i]) * invD;
        if (invD < 0.0f)
        {
            std::swap(t0, t1);
        }
        tmin = t0 > tmin ? t0 : tmin;
        tmax = t1 < tmax ? t1 : tmax;
        if (tmax <= tmin)
        {
            return false;
        }
    }
    return true;
}
