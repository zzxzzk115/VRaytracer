set(TARGET_NAME imgui)
set(IMGUI_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)

file(GLOB imgui_sources CONFIGURE_DEPENDS "${IMGUI_SOURCE_DIR}/*.cpp")

if (PLATFORM_WINDOWS)
    file(GLOB imgui_impl CONFIGURE_DEPENDS
            "${IMGUI_SOURCE_DIR}/backends/imgui_impl_win32.cpp"
            "${IMGUI_SOURCE_DIR}/backends/imgui_impl_win32.h"
            "${IMGUI_SOURCE_DIR}/backends/imgui_impl_dx11.cpp"
            "${IMGUI_SOURCE_DIR}/backends/imgui_impl_dx11.h"
            "${IMGUI_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"
            "${IMGUI_SOURCE_DIR}/backends/imgui_impl_glfw.h"
            "${IMGUI_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
            "${IMGUI_SOURCE_DIR}/backends/imgui_impl_opengl3.h")
else ()
    file(GLOB imgui_impl CONFIGURE_DEPENDS
            "${IMGUI_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"
            "${IMGUI_SOURCE_DIR}/backends/imgui_impl_glfw.h"
            "${IMGUI_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
            "${IMGUI_SOURCE_DIR}/backends/imgui_impl_opengl3.h")
endif ()

include_directories(${GLAD_INCLUDE_DIR})

add_library(${TARGET_NAME} STATIC ${imgui_sources} ${imgui_impl})
add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLAD)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${IMGUI_SOURCE_DIR}>)
target_link_libraries(${TARGET_NAME} PUBLIC glfw glad)
if (PLATFORM_WINDOWS)
    target_link_libraries(${TARGET_NAME} PUBLIC d3d11.lib)
    target_link_libraries(${TARGET_NAME} PUBLIC d3dcompiler.lib)
    target_link_libraries(${TARGET_NAME} PUBLIC dxgi.lib)
    target_link_libraries(${TARGET_NAME} PUBLIC dwmapi.lib)
    target_link_libraries(${TARGET_NAME} PUBLIC dxguid.lib)
    target_link_libraries(${TARGET_NAME} PUBLIC kernel32.lib)
    target_link_libraries(${TARGET_NAME} PUBLIC user32.lib)
    target_link_libraries(${TARGET_NAME} PUBLIC gdi32.lib)
    target_link_libraries(${TARGET_NAME} PUBLIC winspool.lib)
    target_link_libraries(${TARGET_NAME} PUBLIC comdlg32.lib)
    target_link_libraries(${TARGET_NAME} PUBLIC advapi32.lib)
    target_link_libraries(${TARGET_NAME} PUBLIC shell32.lib)
    target_link_libraries(${TARGET_NAME} PUBLIC ole32.lib)
    target_link_libraries(${TARGET_NAME} PUBLIC oleaut32.lib)
    target_link_libraries(${TARGET_NAME} PUBLIC uuid.lib)
    target_link_libraries(${TARGET_NAME} PUBLIC odbc32.lib)
    target_link_libraries(${TARGET_NAME} PUBLIC odbccp32.lib)
endif ()