# SDL3 setup based on consumption method

if(LAYA_SDL_METHOD STREQUAL "submodule")
    # Git submodule method (default)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL/CMakeLists.txt")
        message(FATAL_ERROR "SDL submodule not found. Run: git submodule update --init --recursive")
    endif()

    # Configure SDL3 options
    set(SDL_SHARED OFF CACHE BOOL "Build SDL3 as shared library" FORCE)
    set(SDL_STATIC ON CACHE BOOL "Build SDL3 as static library" FORCE)
    set(SDL_TEST OFF CACHE BOOL "Build SDL3 tests" FORCE)

    add_subdirectory(external/SDL)

    # Create aliases
    if(NOT TARGET SDL3::SDL3)
        add_library(SDL3::SDL3 ALIAS SDL3-static)
    endif()

    # Handle SDL3_image
    if(LAYA_USE_SDL_IMAGE)
        if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL_image/CMakeLists.txt")
            message(FATAL_ERROR "SDL_image submodule not found. Run: git submodule update --init --recursive")
        endif()

        # Disable SDL3_image installation and tests
        set(SDL3IMAGE_INSTALL OFF CACHE BOOL "" FORCE)
        set(SDL3IMAGE_SAMPLES OFF CACHE BOOL "" FORCE)
        set(SDL3IMAGE_TESTS OFF CACHE BOOL "" FORCE)

        # Skip find_package for SDL3 in SDL3_image since we're building it
        set(SDL3IMAGE_DEPS_SHARED OFF CACHE BOOL "" FORCE)

        add_subdirectory(external/SDL_image)

        if(NOT TARGET SDL3::SDL3_image)
            add_library(SDL3::SDL3_image ALIAS SDL3_image)
        endif()
    endif()

    # Handle SDL3_ttf
    if(LAYA_USE_SDL_TTF)
        if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL_ttf/CMakeLists.txt")
            message(FATAL_ERROR "SDL_ttf submodule not found. Run: git submodule update --init --recursive")
        endif()

        # Disable SDL3_ttf installation and tests
        set(SDL3TTF_INSTALL OFF CACHE BOOL "" FORCE)
        set(SDL3TTF_SAMPLES OFF CACHE BOOL "" FORCE)

        # Skip find_package for SDL3 in SDL3_ttf since we're building it
        set(SDL3TTF_DEPS_SHARED OFF CACHE BOOL "" FORCE)

        add_subdirectory(external/SDL_ttf)

        if(NOT TARGET SDL3::SDL3_ttf)
            add_library(SDL3::SDL3_ttf ALIAS SDL3_ttf)
        endif()
    endif()

elseif(LAYA_SDL_METHOD STREQUAL "system")
    # System-installed SDL3
    find_package(SDL3 REQUIRED)

    if(LAYA_USE_SDL_IMAGE)
        find_package(SDL3_image REQUIRED)
    endif()

    if(LAYA_USE_SDL_TTF)
        find_package(SDL3_ttf REQUIRED)
    endif()

elseif(LAYA_SDL_METHOD STREQUAL "vcpkg")
    # vcpkg package manager
    find_package(SDL3 CONFIG REQUIRED)

    if(LAYA_USE_SDL_IMAGE)
        find_package(SDL3-image CONFIG REQUIRED)

        if(NOT TARGET SDL3::SDL3_image)
            add_library(SDL3::SDL3_image ALIAS SDL3_image::SDL3_image)
        endif()
    endif()

    if(LAYA_USE_SDL_TTF)
        find_package(SDL3-ttf CONFIG REQUIRED)

        if(NOT TARGET SDL3::SDL3_ttf)
            add_library(SDL3::SDL3_ttf ALIAS SDL3_ttf::SDL3_ttf)
        endif()
    endif()

else()
    message(FATAL_ERROR "Unknown SDL method: ${LAYA_SDL_METHOD}")
endif()

# Verify required targets exist
if(NOT TARGET SDL3::SDL3)
    message(FATAL_ERROR "SDL3::SDL3 target not found")
endif()

if(LAYA_USE_SDL_IMAGE AND NOT TARGET SDL3::SDL3_image)
    message(FATAL_ERROR "SDL3::SDL3_image target not found")
endif()

if(LAYA_USE_SDL_TTF AND NOT TARGET SDL3::SDL3_ttf)
    message(FATAL_ERROR "SDL3::SDL3_ttf target not found")
endif()
