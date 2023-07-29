set(TARGET_NAME VRaytracer)
set(TARGET_BINARY_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET_NAME})

add_subdirectory(ThirdParty)

add_executable(${TARGET_NAME} main.cpp "Platform.h" "Log.h" "Base.h" "Log.cpp" "Raytracer.h" "Macro.h" "IRuntimeModule.h" "UIModule.h" "UIModule.cpp" "Raytracer.cpp" "Window.h" "Window.cpp" "RaytracerCore.h" "Event.h" "Renderer.h" "Renderer.cpp" "Configuration.h" "FileSystem.h" "FileSystem.cpp")

# Set output path
set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TARGET_BINARY_DIR})
if (MSVC)
    set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${TARGET_BINARY_DIR})
    set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${TARGET_BINARY_DIR})
endif ()

# Copy imgui config
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/AppData/imgui.ini DESTINATION ${TARGET_BINARY_DIR})

# Copy Resources
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Resources DESTINATION ${TARGET_BINARY_DIR})

# being a cross-platform target, we enforce standards conformance on MSVC
target_compile_options(${TARGET_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive->")
target_compile_options(${TARGET_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->")

# Link dependencies
target_link_libraries(${TARGET_NAME} PUBLIC spdlog)
target_link_libraries(${TARGET_NAME} PUBLIC glad)
target_link_libraries(${TARGET_NAME} PUBLIC glfw)
target_link_libraries(${TARGET_NAME} PUBLIC imgui)
target_link_libraries(${TARGET_NAME} PUBLIC stb)
target_link_libraries(${TARGET_NAME} PUBLIC args)
target_link_libraries(${TARGET_NAME} PUBLIC cereal)

target_include_directories(
    ${TARGET_NAME}
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/${TARGET_NAME}-${PROJECT_VERSION}>
)