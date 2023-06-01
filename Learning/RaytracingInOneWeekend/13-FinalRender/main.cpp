#include "Common.h"
#include "Color.h"
#include "HittableList.h"
#include "Sphere.h"
#include "Camera.h"
#include "Material.h"

#include <iostream>

// Image
const double AspectRatio     = 3.0 / 2.0;
const int    ImageWidth      = 1200;
const int    ImageHeight     = static_cast<int>(ImageWidth / AspectRatio);
const int    SamplesPerPixel = 500;
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

HittableList GetRandomScene()
{
	HittableList world;

    auto materialGround = std::make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
    world.Add(std::make_shared<Sphere>(Point3(0, -1000, 0), 1000, materialGround));

    for (int a = -11; a < 11; a++)
    {
        for (int b = -11; b < 11; b++)
        {
            double materialChosen = GetRandomDouble();
            Point3 center(a + 0.9 * GetRandomDouble(), 0.2, b + 0.9 * GetRandomDouble());

            if ((center - Point3(4, 0.2, 0)).Length() > 0.9)
            {
                std::shared_ptr<Material> materialSphere;

                if (materialChosen < 0.8)
                {
	                // Diffuse
                    Color albedo = Color::GetRandom() * Color::GetRandom();
                    materialSphere = std::make_shared<Lambertian>(albedo);
                    
                }
                else if (materialChosen < 0.95)
                {
	                // Metal
                    Color albedo = Color::GetRandom(0.5, 1);
                    double fuzz   = GetRandomDouble(0, 0.5);
                    materialSphere = std::make_shared<Metal>(albedo, fuzz);
                }
                else
                {
	                // Glass
                    materialSphere = std::make_shared<Dielectric>(1.5);
                }

                world.Add(std::make_shared<Sphere>(center, 0.2, materialSphere));
            }
        }
    }

    auto material1 = std::make_shared<Dielectric>(1.5);
    world.Add(std::make_shared<Sphere>(Point3(0, 1, 0), 1.0, material1));

    auto material2 = std::make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
    world.Add(std::make_shared<Sphere>(Point3(-4, 1, 0), 1.0, material2));

    auto material3 = std::make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
    world.Add(std::make_shared<Sphere>(Point3(4, 1, 0), 1.0, material3));

    return world;
}

int main()
{
    // World
    auto world = GetRandomScene();

    // Camera
    Point3  lookFrom(13, 2, 3);
    Point3  lookAt(0, 0, 0);
    Vector3 viewUp(0, 1, 0);
    double  distanceToFocus = 10.0;
    double  aperture        = 0.1;
    Camera  cam(lookFrom, lookAt, viewUp, 20.0, AspectRatio, aperture, distanceToFocus);

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