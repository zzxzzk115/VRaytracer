#pragma once

#include "Log.h"

#define VRT_TRACE(...) ::VRaytracer::Log::GetLogger()->trace(__VA_ARGS__)
#define VRT_INFO(...) ::VRaytracer::Log::GetLogger()->info(__VA_ARGS__)
#define VRT_WARN(...) ::VRaytracer::Log::GetLogger()->warn(__VA_ARGS__)
#define VRT_ERROR(...) ::VRaytracer::Log::GetLogger()->error(__VA_ARGS__)
#define VRT_CRITICAL(...) ::VRaytracer::Log::GetLogger()->critical(__VA_ARGS__)