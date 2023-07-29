set(TARGET_NAME cereal)

set(CEREAL_THREAD_LIBS)
if(UNIX)
    option(THREAD_SAFE "Use mutexes to ensure thread safety" OFF)
    if(THREAD_SAFE)
        message(STATUS "Use mutexes")
        add_definitions(-DCEREAL_THREAD_SAFE=1)
        set(CEREAL_THREAD_LIBS pthread)
    endif()
endif()

if(MSVC)
    add_compile_options(/bigobj /W3 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Wshadow -Wold-style-cast)
    option(WITH_WERROR "Compile with '-Werror' C++ compiler flag" ON)
    if(WITH_WERROR)
        add_compile_options(-Werror)
    endif()

    option(CLANG_USE_LIBCPP "Use libc++ for clang compilation" OFF)
    if(APPLE OR CLANG_USE_LIBCPP)
        message(STATUS "Use libc++")
        add_compile_options(-stdlib=libc++)
        # TODO: use add_link_options(-stdlib=libc++ -lc++abi") bud this needs cmake 3.13! CK
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
    endif()

    if(NOT DEFINED CMAKE_CXX_STANDARD OR CMAKE_CXX_STANDARD STREQUAL "98")
        set(CMAKE_CXX_STANDARD 11)
    endif()

    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

add_library(${TARGET_NAME} INTERFACE)
target_include_directories(${TARGET_NAME} INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)