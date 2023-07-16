#pragma once

// Detect Windows
#if defined(_WIN32) || defined(_WIN64)
#define VRT_WINDOWS
#endif

// Detect macOS
#if defined(__APPLE__) && defined(__MACH__)
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR == 1
#define VRT_IOS_SIMULATOR
#elif TARGET_OS_IPHONE == 1
#define VRT_IOS
#elif TARGET_OS_MAC == 1
#define VRT_MACOS
#else
#error "Unknown Apple platform"
#endif
#endif

// Detect Linux
#if defined(__linux__)
#define VRT_LINUX
#endif

// Detect Android
#if defined(__ANDROID__)
#define VRT_ANDROID
#endif