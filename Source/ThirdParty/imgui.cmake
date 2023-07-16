set(TARGET_NAME imgui)
set(IMGUI_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)

file(GLOB imgui_sources CONFIGURE_DEPENDS "${IMGUI_SOURCE_DIR}/*.cpp")

file(GLOB imgui_impl CONFIGURE_DEPENDS
    "${IMGUI_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"
    "${IMGUI_SOURCE_DIR}/backends/imgui_impl_glfw.h"
    "${IMGUI_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
    "${IMGUI_SOURCE_DIR}/backends/imgui_impl_opengl3.h")

include_directories(${GLAD_INCLUDE_DIR})

add_library(${TARGET_NAME} STATIC ${imgui_sources} ${imgui_impl})
add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLAD)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${IMGUI_SOURCE_DIR}>)
target_link_libraries(${TARGET_NAME} PUBLIC glfw glad)