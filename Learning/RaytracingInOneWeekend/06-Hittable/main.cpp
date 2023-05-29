#include "Common.h"
#include "Color.h"
#include "HittableList.h"
#include "Sphere.h"

#include <iostream>

// Image
const double AspectRatio = 16.0 / 9.0;
const int    ImageWidth  = 400;
const int    ImageHeight = static_cast<int>(ImageWidth / AspectRatio);

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
    const double viewPortHeight = 2.0;
    const double viewPortWidth  = AspectRatio * viewPortHeight;
    const double focalLength    = 1.0;

    const auto origin = Point3(0, 0, 0);
    const auto horizontal = Vector3(viewPortWidth, 0, 0);
    const auto vertical   = Vector3(0, viewPortHeight, 0);
    const auto lowerLeftCorner = origin - horizontal / 2 - vertical / 2 - Vector3(0, 0, focalLength);

    // Render
    std::cout << "P3\n" << ImageWidth << ' ' << ImageHeight << "\n255\n";

    for (int j = ImageHeight - 1; j >= 0; --j)
    {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < ImageWidth; ++i)
        {
            double  u = static_cast<double>(i) / (ImageWidth - 1);
            double  v = static_cast<double>(j) / (ImageHeight - 1);
            Ray   r(origin, lowerLeftCorner + u * horizontal + v * vertical - origin);
            const Color pixelColor = GetRayColor(r, world);
            WriteColor(std::cout, pixelColor);
        }
    }

    std::cerr << "\nDone.\n";
    return 0;
}