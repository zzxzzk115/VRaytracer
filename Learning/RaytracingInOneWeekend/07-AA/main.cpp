#include "Common.h"
#include "Color.h"
#include "HittableList.h"
#include "Sphere.h"
#include "Camera.h"

#include <iostream>

// Image
const double AspectRatio     = 16.0 / 9.0;
const int    ImageWidth      = 400;
const int    ImageHeight     = static_cast<int>(ImageWidth / AspectRatio);
const int    SamplesPerPixel = 100;

Color GetRayColor(const Ray& r, const Hittable& world)
{
    HitRecord rec;
    if (world.Hit(r, 0, Infinity, rec))
    {
        return 0.5 * (rec.Normal + Color(1, 1, 1));
    }

    Vector3 unitDirection = Normalize(r.Direction());
    double  t             = 0.5 * (unitDirection.y() + 1.0);
    return (1.0 - t) * White + t * LightBlue;
}

int main()
{
    // World
    HittableList world;
    world.Add(std::make_shared<Sphere>(Point3(0, 0, -1), 0.5));
    world.Add(std::make_shared<Sphere>(Point3(0, -100.5, -1), 100));

    // Camera
    Camera cam;

    // Render
    std::cout << "P3\n" << ImageWidth << ' ' << ImageHeight << "\n255\n";

    for (int j = ImageHeight - 1; j >= 0; --j)
    {
        for (int i = 0; i < ImageWidth; ++i)
        {
            Color pixelColor(0, 0, 0);
            for (int s = 0; s < SamplesPerPixel; ++s)
            {
                double u = (i + GetRandomDouble()) / (ImageWidth - 1);
                double v = (j + GetRandomDouble()) / (ImageHeight - 1);
                Ray    r = cam.GetRay(u, v);
                pixelColor += GetRayColor(r, world);
            }
            WriteColor(std::cout, pixelColor, SamplesPerPixel);
        }
    }

    return 0;
}