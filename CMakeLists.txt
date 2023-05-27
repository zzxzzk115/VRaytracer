cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(VRaytracer VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

option(BUILD_LEARNING "Build learning codes" ON)

include(CMakeDependentOption)
include(CMake/WSL.cmake)

# Include guards
if (PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(
            FATAL_ERROR
            "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
    )
endif ()

# Platform Detection
is_wsl(IS_WSL)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message("Running on Windows")
    set(PLATFORM_WINDOWS ON)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    if (IS_WSL)
        message("Running on WSL")
        set(PLATFORM_WSL ON)
    else ()
        message("Running on Linux")
        set(PLATFORM_LINUX ON)
    endif ()
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message("Running on macOS")
    set(PLATFORM_DARWIN ON)
else ()
    message(FATAL_ERROR "Not supported yet")
endif ()

# Arch Detection
if (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" OR
        CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64" OR
        CMAKE_SYSTEM_PROCESSOR STREQUAL "x64")
    set(ARCH "x64")
    message("Architecture: ${ARCH}")
elseif(CMAKE_SYSTEM_PROCESSOR  STREQUAL "x86" OR
        CMAKE_SYSTEM_PROCESSOR  STREQUAL "i386" OR
        CMAKE_SYSTEM_PROCESSOR  STREQUAL "i686")
    set(ARCH "x86")
    message("Architecture: ${ARCH}")
else()
    message(FATAL_ERROR "Unsupported Arch: ${CMAKE_SYSTEM_PROCESSOR}")
endif()

set(VRAYTRACER_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_INSTALL_PREFIX "${VRAYTRACER_ROOT_DIR}/bin")
set(BINARY_ROOT_DIR "${CMAKE_INSTALL_PREFIX}/")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${VRAYTRACER_ROOT_DIR}/bin)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

if (BUILD_LEARNING)
    add_subdirectory(Learning)
endif()