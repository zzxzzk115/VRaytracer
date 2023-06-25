#pragma once

#include "Common.h"

#include "Hittable.h"
#include "HittableList.h"

#include <algorithm>

class BVHNode : public Hittable
{
public:
    BVHNode() {}
    BVHNode(const HittableList& list, double time0, double time1) :
        BVHNode(list.GetObjects(), 0, list.GetObjects().size(), time0, time1)
    {}
    BVHNode(const std::vector<std::shared_ptr<Hittable>>& srcObjects,
            size_t                                        start,
            size_t                                        end,
            double                                        time0,
            double                                        time1);

    virtual bool Hit(const Ray& r, double tMin, double tMax, HitRecord& rec) override
    {
        if (!m_Box.Hit(r, tMin, tMax))
            return false;

        bool hitLeft = m_Left->Hit(r, tMin, tMax, rec);
        bool hitRight = m_Right->Hit(r, tMin, hitLeft ? rec.T : tMax, rec);

        return hitLeft || hitRight;
    }

    virtual bool BoundingBox(double time0, double time1, AABB& outputBox) override
    {
        outputBox = m_Box;
        return true;
    }

private:
    std::shared_ptr<Hittable> m_Left;
    std::shared_ptr<Hittable> m_Right;
    AABB                      m_Box;
};

BVHNode::BVHNode(const std::vector<std::shared_ptr<Hittable>>& srcObjects,
                 size_t                                        start,
                 size_t                                        end,
                 double                                        time0,
                 double                                        time1)
{
    auto objects = srcObjects; // Create a modifiable array of the source scene objects

    // chose an axis to compare
    int axis = GetRandomInt(0, 2);

    auto comparator = (axis == 0) ? BoxCompareX : (axis == 1) ? BoxCompareY : BoxCompareZ;

    size_t objectSpan = end - start;

    if (objectSpan == 1)
    {
        m_Left = m_Right = objects[start];
    }
    else if (objectSpan == 2)
    {
        if (comparator(objects[start], objects[start+1]))
        {
            m_Left  = objects[start];
            m_Right = objects[start + 1];
        }
        else
        {
            m_Left  = objects[start + 1];
            m_Right = objects[start];
        }
    }
    else
    {
        std::sort(objects.begin() + start, objects.begin() + end, comparator);

        auto mid = start + objectSpan / 2;
        m_Left   = std::make_shared<BVHNode>(objects, start, mid, time0, time1);
        m_Right  = std::make_shared<BVHNode>(objects, mid, end, time0, time1);
    }

    AABB boxLeft, boxRight;

    if (!m_Left->BoundingBox(time0, time1, boxLeft) || !m_Right->BoundingBox(time0, time1, boxRight))
    {
        std::cerr << "No bounding box in BVHNode constructor." << std::endl;
    }

    m_Box = GetSurroundingBox(boxLeft, boxRight);
}

inline bool BoxCompare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b, int axis)
{
    AABB boxA, boxB;

    if (!a->BoundingBox(0, 0, boxA) || !b->BoundingBox(0, 0, boxB))
        std::cerr << "No bounding box in BVHNode constructor." << std::endl;

    return boxA.GetMin()[axis] < boxB.GetMin()[axis];
}

inline bool BoxCompareX(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b)
{
    return BoxCompare(a, b, 0);
}

inline bool BoxCompareY(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b)
{
    return BoxCompare(a, b, 1);
}

inline bool BoxCompareZ(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b)
{
    return BoxCompare(a, b, 2);
}