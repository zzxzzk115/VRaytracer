set(TARGET_NAME 03-SolidTextures)
set(TARGET_BINARY_DIR ${RAYTRACING_THE_NEXT_WEEK_BINARY_ROOT}/${TARGET_NAME})

# Copy Scripts
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/run.bat DESTINATION ${TARGET_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/run.sh DESTINATION ${TARGET_BINARY_DIR})

add_executable(${TARGET_NAME} main.cpp "Hittable.h" "Sphere.h" "HittableList.h" "Common.h" "Camera.h"  "MovingSphere.h" "AABB.h" "BVH.h" "Texture.h")

# Add OpenMP support
target_include_directories(${TARGET_NAME} PRIVATE ${OpenMP_CXX_INCLUDE_DIRS})
target_link_libraries(${TARGET_NAME} PRIVATE ${OpenMP_CXX_LIBRARIES})
target_compile_options(${TARGET_NAME} PRIVATE ${OpenMP_CXX_FLAGS})

# Set output path
set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TARGET_BINARY_DIR})
if (MSVC)
    set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${TARGET_BINARY_DIR})
    set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${TARGET_BINARY_DIR})
endif ()