#include "objects/Bvh.h"

#include <algorithm>
#include <iostream>
#include <random>

BvhNode::BvhNode(std::vector<std::shared_ptr<Hitable>>::iterator begin, std::vector<std::shared_ptr<Hitable>>::iterator end)
{
    int size = end - begin;
    std::uniform_int_distribution dis(0, 2);
    std::mt19937 ran(size);
    int axis = dis(ran);
    std::stable_sort(begin, end, [axis](std::shared_ptr<Hitable> a, std::shared_ptr<Hitable> b)
    {
        Aabb box_a, box_b;
        // TODO: segmentation fault
        if (!(a->bounding_box(box_a) && b->bounding_box(box_b)))
        {
            std::cerr << "No bounding box for hitable that should be sorted!" << std::endl;
        }
        if (box_a.min()[axis] > box_b.min()[axis])
        {
            return false;
        }
        else
        {
            return true;
        }
    });
    if (size == 1)
    {
        left = right = begin[0];
    }
    else if (size == 2)
    {
        left = begin[0];
        right = begin[1];
    }
    else
    {
        left = std::make_shared<BvhNode>(begin, begin + size / 2);
        right = std::make_shared<BvhNode>(begin + size / 2, end);
    }
    Aabb box_left, box_right;
    if (!(left->bounding_box(box_left) && right->bounding_box(box_right)))
    {
        std::cerr << "No bounding box for left or right child node!" << std::endl;
    }
    box = Aabb(box_left, box_right);
}

bool BvhNode::bounding_box(Aabb& b) const
{
    b = box;
    return true;
}

bool BvhNode::hit(const Ray& r, float tmin, float tmax, RayPayload& rp) const
{
    if (box.hit(r, tmin, tmax))
    {
        RayPayload left_rec(rp.depth, rp.wavelength), right_rec(rp.depth, rp.wavelength);
        bool hit_left = left->hit(r, tmin, tmax, left_rec);
        bool hit_right = right->hit(r, tmin, tmax, right_rec);
        if (hit_left && hit_right)
        {
            if (left_rec.t < right_rec.t)
            {
                rp = left_rec;
            }
            else
            {
                rp = right_rec;
            }
            return true;
        }
        else if (hit_left)
        {
            rp = left_rec;
            return true;
        }
        else if (hit_right)
        {
            rp = right_rec;
            return true;
        }
    }
    return false;
}
