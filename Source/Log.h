#pragma once

#include "Base.h"

// This ignores all warnings raised inside External headers
// clang-format off
#pragma warning(push, 0)

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#pragma warning(pop)
// clang-format on

namespace VRaytracer
{
    class Log
    {
    public:
        static void Init();
        static Ref<spdlog::logger>& GetLogger() { return s_Logger; }

    private:
        static Ref<spdlog::logger> s_Logger;
    };
} // namespace VRaytracer