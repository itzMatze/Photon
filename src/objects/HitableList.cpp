#include "objects/HitableList.h"

bool HitableList::hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const
{
    HitRecord temp_rec;
    bool hit_anything = false;
    float closest_so_far = t_max;
    for (const auto& i: *objects)
    {
        if (i->hit(r, t_min, closest_so_far, temp_rec))
        {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }
    return hit_anything;
}

bool HitableList::bounding_box(Aabb& box) const
{
    if (objects->size() < 1) return false;
    Aabb temp_box;
    bool first_true = (*objects)[0]->bounding_box(temp_box);
    if (!first_true)
    {
        return false;
    }
    else
    {
        box = temp_box;
    }
    for (int i = 1; i < objects->size(); ++i)
    {
        if ((*objects)[i]->bounding_box(temp_box))
        {
            box = Aabb(box, temp_box);
        }
        else
        {
            return false;
        }
    }
    return true;
}
