set(THIRD_PARTY_FOLDER "ThirdParty")

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

if (NOT TARGET glad)
    add_subdirectory(glad)
    set_target_properties(glad PROPERTIES FOLDER ${THIRD_PARTY_FOLDER}/glad)
endif ()

if (NOT TARGET glfw)
    option(GLFW_INSTALL "" OFF)
    add_subdirectory(glfw)
    set_target_properties(glfw PROPERTIES FOLDER ${THIRD_PARTY_FOLDER}/glfw)
    set_target_properties(update_mappings PROPERTIES FOLDER ${THIRD_PARTY_FOLDER}/glfw)
endif ()

if (NOT TARGET imgui)
    include(imgui.cmake)
    set_target_properties(imgui PROPERTIES FOLDER ${THIRD_PARTY_FOLDER}/imgui)
endif ()

if (NOT TARGET spdlog)
    option(SPDLOG_INSTALL "" OFF)
    add_subdirectory(spdlog)
    set_target_properties(spdlog PROPERTIES FOLDER ${THIRD_PARTY_FOLDER}/spdlog)
endif ()

if (NOT TARGET stb)
    add_subdirectory(stb)
    set_target_properties(stb PROPERTIES FOLDER ${THIRD_PARTY_FOLDER}/stb)
endif ()

if (NOT TARGET args)
    add_subdirectory(args)
    set_target_properties(args PROPERTIES FOLDER ${THIRD_PARTY_FOLDER}/args)
endif ()