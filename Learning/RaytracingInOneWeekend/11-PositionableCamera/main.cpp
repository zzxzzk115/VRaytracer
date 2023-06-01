#include "Common.h"
#include "Color.h"
#include "HittableList.h"
#include "Sphere.h"
#include "Camera.h"
#include "Material.h"

#include <iostream>

// Image
const double AspectRatio     = 16.0 / 9.0;
const int    ImageWidth      = 400;
const int    ImageHeight     = static_cast<int>(ImageWidth / AspectRatio);
const int    SamplesPerPixel = 100;
const int    MaxDepth        = 50;

Color GetRayColor(const Ray& r, const Hittable& world, int depth)
{
    HitRecord rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return Black;

    if (world.Hit(r, 0.001, Infinity, rec))
    {
        Ray    scattered;
        Color  attenuation;
        if (rec.MaterialPtr->Scatter(r, rec, attenuation, scattered))
            return attenuation * GetRayColor(scattered, world, depth - 1);
        return Black;
    }

    Vector3 unitDirection = Normalize(r.Direction());
    double  t             = 0.5 * (unitDirection.y() + 1.0);
    return (1.0 - t) * White + t * LightBlue;
}

int main()
{
    // World
    HittableList world;

    auto         materialGround = std::make_shared<Lambertian>(Color(0.8, 0.8, 0.0));
    auto         materialCenter = std::make_shared<Lambertian>(Color(0.1, 0.2, 0.5));
    auto         materialLeft   = std::make_shared<Dielectric>(1.5);
    auto         materialRight  = std::make_shared<Metal>(Color(0.8, 0.6, 0.2), 0.0);

    world.Add(std::make_shared<Sphere>(Point3(0.0, -100.5, -1.0), 100.0, materialGround));
    world.Add(std::make_shared<Sphere>(Point3(0.0, 0.0, -1.0), 0.5, materialCenter));
    world.Add(std::make_shared<Sphere>(Point3(-1.0, 0.0, -1.0), 0.5, materialLeft));
    // Negative Radius -> trick for hollow glass.
    world.Add(std::make_shared<Sphere>(Point3(-1.0, 0.0, -1.0), -0.4, materialLeft));
    world.Add(std::make_shared<Sphere>(Point3(1.0, 0.0, -1.0), 0.5, materialRight));

    // Camera
    Camera cam(Point3(-2, 2, 1), Point3(0, 0, -1), Vector3(0, 1, 0), 20, AspectRatio);

    // Render
    std::cout << "P3\n" << ImageWidth << ' ' << ImageHeight << "\n255\n";

    for (int j = ImageHeight - 1; j >= 0; --j)
    {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < ImageWidth; ++i)
        {
            Color pixelColor(0, 0, 0);
            for (int s = 0; s < SamplesPerPixel; ++s)
            {
                double u = (i + GetRandomDouble()) / (ImageWidth - 1);
                double v = (j + GetRandomDouble()) / (ImageHeight - 1);
                Ray    r = cam.GetRay(u, v);
                pixelColor += GetRayColor(r, world, MaxDepth);
            }
            WriteColor(std::cout, pixelColor, SamplesPerPixel);
        }
    }

    std::cerr << "\nDone.\n";
    return 0;
}