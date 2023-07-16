include_directories(include)

add_library(glad src/glad.c)
target_include_directories(glad PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
set_target_properties(glad PROPERTIES
                      POSITION_INDEPENDENT_CODE ON)