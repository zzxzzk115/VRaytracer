#include "Log.h"
#include "Platform.h"
#include "Raytracer.h"
#include "FileSystem.h"

#include <args.hxx>

#ifdef VRT_WINDOWS
#include <Windows.h>
#endif

using namespace VRaytracer;

int main(int argc, char** argv)
{
#ifdef VRT_WINDOWS
    SetConsoleOutputCP(65001);
#endif

    Log::Init();
    FileSystem::InitExecutableDirectory(argv[0]);
    RaytracerConfiguration config;
    Raytracer              raytracer(config);

    raytracer.Run();

    return 0;
}