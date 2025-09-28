# SDL3 setup based on consumption method

# Skip SDL setup if parent project provides targets
if(LAYA_SDL_TARGETS_PROVIDED)
    # Verify that required targets exist
    if(NOT TARGET SDL3::SDL3)
        message(FATAL_ERROR "LAYA_SDL_TARGETS_PROVIDED is ON but SDL3::SDL3 target not found")
    endif()

    if(LAYA_USE_SDL_IMAGE AND NOT TARGET SDL3::SDL3_image)
        message(FATAL_ERROR "LAYA_USE_SDL_IMAGE is ON but SDL3::SDL3_image target not found")
    endif()

    if(LAYA_USE_SDL_TTF AND NOT TARGET SDL3::SDL3_ttf)
        message(FATAL_ERROR "LAYA_USE_SDL_TTF is ON but SDL3::SDL3_ttf target not found")
    endif()

    message(STATUS "Using SDL targets provided by parent project")
    return()
endif()

if(LAYA_SDL_METHOD STREQUAL "submodule")
    # Git submodule method (default)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL/CMakeLists.txt")
        message(FATAL_ERROR "SDL submodule not found. Run: git submodule update --init --recursive")
    endif()

    # Configure SDL3 options - Build as shared libraries for better compatibility
    set(SDL_SHARED ON CACHE BOOL "Build SDL3 as shared library" FORCE)
    set(SDL_STATIC OFF CACHE BOOL "Build SDL3 as static library" FORCE)
    set(SDL_TEST OFF CACHE BOOL "Build SDL3 tests" FORCE)

    # Create SDL3Config.cmake BEFORE adding SDL subdirectory
    # This ensures SDL3_image can find it during configuration
    set(SDL3_CONFIG_DIR "${CMAKE_CURRENT_BINARY_DIR}/SDL3Config")
    file(MAKE_DIRECTORY "${SDL3_CONFIG_DIR}")

    # Set SDL3_DIR early so it's available when SDL3_image configures
    set(SDL3_DIR "${SDL3_CONFIG_DIR}" CACHE PATH "Path to SDL3Config.cmake" FORCE)

    # Write a basic SDL3Config.cmake that SDL3_image can use
    file(WRITE "${SDL3_CONFIG_DIR}/SDL3Config.cmake" "
# Generated SDL3Config.cmake for submodule build
set(SDL3_FOUND TRUE)
set(SDL3_VERSION \"3.2.6\")

# Create aliases to the actual targets that will be created by add_subdirectory
# Since we're building shared libraries, SDL3::SDL3 should point to the shared target
if(TARGET SDL3-shared AND NOT TARGET SDL3::SDL3)
    add_library(SDL3::SDL3 ALIAS SDL3-shared)
endif()

if(TARGET SDL3-headers AND NOT TARGET SDL3::Headers)
    add_library(SDL3::Headers ALIAS SDL3-headers)
endif()

if(TARGET SDL3-shared AND NOT TARGET SDL3::SDL3-shared)
    add_library(SDL3::SDL3-shared ALIAS SDL3-shared)
endif()

# Mark components as found
set(SDL3_Headers_FOUND TRUE)
set(SDL3_SDL3-shared_FOUND TRUE)
")

    add_subdirectory(external/SDL)

    # Ensure SDL3 targets are properly available for SDL3_image
    # SDL3_image expects these targets to exist
    if(NOT TARGET SDL3::SDL3 AND TARGET SDL3-shared)
        add_library(SDL3::SDL3 ALIAS SDL3-shared)
    endif()

    if(NOT TARGET SDL3::Headers)
        add_library(SDL3::Headers ALIAS SDL3-headers)
    endif()

    if(NOT TARGET SDL3::SDL3-shared AND TARGET SDL3-shared)
        add_library(SDL3::SDL3-shared ALIAS SDL3-shared)
    endif()

    # Set variables that SDL3_image's find_package expects
    set(SDL3_FOUND TRUE)
    set(SDL3_VERSION "3.2.6")

    # Handle SDL3_image
    if(LAYA_USE_SDL_IMAGE)
        if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL_image/CMakeLists.txt")
            message(FATAL_ERROR "SDL_image submodule not found. Run: git submodule update --init --recursive")
        endif()

        # Build SDL3_image as shared library
        set(SDL3IMAGE_SHARED ON CACHE BOOL "Build SDL3_image as shared library" FORCE)
        set(SDL3IMAGE_STATIC OFF CACHE BOOL "Build SDL3_image as static library" FORCE)

        # Disable SDL3_image installation and tests
        set(SDL3IMAGE_INSTALL OFF CACHE BOOL "" FORCE)
        set(SDL3IMAGE_SAMPLES OFF CACHE BOOL "" FORCE)
        set(SDL3IMAGE_TESTS OFF CACHE BOOL "" FORCE)

        # Skip find_package for SDL3 in SDL3_image since we're building it
        set(SDL3IMAGE_DEPS_SHARED OFF CACHE BOOL "" FORCE)

        # Disable vendored dependencies that aren't available
        set(SDLIMAGE_VENDORED ON CACHE BOOL "" FORCE)
        set(SDLIMAGE_AVIF OFF CACHE BOOL "" FORCE)
        set(SDLIMAGE_JXL OFF CACHE BOOL "" FORCE)
        set(SDLIMAGE_WEBP OFF CACHE BOOL "" FORCE)

        add_subdirectory(external/SDL_image)

        # SDL3_image creates different target names based on shared/static build
        if(NOT TARGET SDL3::SDL3_image)
            if(TARGET SDL3_image-shared)
                add_library(SDL3::SDL3_image ALIAS SDL3_image-shared)
            elseif(TARGET SDL3_image-static)
                add_library(SDL3::SDL3_image ALIAS SDL3_image-static)
            elseif(TARGET SDL3_image)
                add_library(SDL3::SDL3_image ALIAS SDL3_image)
            endif()
        endif()
    endif()

    # Handle SDL3_ttf
    if(LAYA_USE_SDL_TTF)
        if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL_ttf/CMakeLists.txt")
            message(FATAL_ERROR "SDL_ttf submodule not found. Run: git submodule update --init --recursive")
        endif()

        # Build SDL3_ttf as shared library
        set(SDL3TTF_SHARED ON CACHE BOOL "Build SDL3_ttf as shared library" FORCE)
        set(SDL3TTF_STATIC OFF CACHE BOOL "Build SDL3_ttf as static library" FORCE)

        # Disable SDL3_ttf installation and tests
        set(SDL3TTF_INSTALL OFF CACHE BOOL "" FORCE)
        set(SDL3TTF_SAMPLES OFF CACHE BOOL "" FORCE)

        # Skip find_package for SDL3 in SDL3_ttf since we're building it
        set(SDL3TTF_DEPS_SHARED OFF CACHE BOOL "" FORCE)

        # Disable vendored dependencies that aren't available
        set(SDLTTF_VENDORED ON CACHE BOOL "" FORCE)

        add_subdirectory(external/SDL_ttf)

        # SDL3_ttf creates different target names based on shared/static build
        if(NOT TARGET SDL3::SDL3_ttf)
            if(TARGET SDL3_ttf-shared)
                add_library(SDL3::SDL3_ttf ALIAS SDL3_ttf-shared)
            elseif(TARGET SDL3_ttf-static)
                add_library(SDL3::SDL3_ttf ALIAS SDL3_ttf-static)
            elseif(TARGET SDL3_ttf)
                add_library(SDL3::SDL3_ttf ALIAS SDL3_ttf)
            endif()
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

# Function to copy SDL shared libraries to executable directory and set RPATH
function(laya_copy_sdl_shared_libs target_name)
    if(LAYA_SDL_METHOD STREQUAL "submodule")
        # On Windows, copy DLLs to executable directory
        if(WIN32)
            # Copy SDL3 shared library
            if(TARGET SDL3-shared)
                add_custom_command(TARGET ${target_name} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    $<TARGET_FILE:SDL3-shared>
                    $<TARGET_FILE_DIR:${target_name}>
                    COMMENT "Copying SDL3 shared library to ${target_name} directory"
                )
            endif()

            # Copy SDL3_image shared library
            if(LAYA_USE_SDL_IMAGE AND TARGET SDL3_image-shared)
                add_custom_command(TARGET ${target_name} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    $<TARGET_FILE:SDL3_image-shared>
                    $<TARGET_FILE_DIR:${target_name}>
                    COMMENT "Copying SDL3_image shared library to ${target_name} directory"
                )
            endif()

            # Copy SDL3_ttf shared library
            if(LAYA_USE_SDL_TTF AND TARGET SDL3_ttf-shared)
                add_custom_command(TARGET ${target_name} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    $<TARGET_FILE:SDL3_ttf-shared>
                    $<TARGET_FILE_DIR:${target_name}>
                    COMMENT "Copying SDL3_ttf shared library to ${target_name} directory"
                )
            endif()

        # On Unix-like systems (Linux/macOS), set RPATH and copy libraries
        else()
            # Set RPATH to look for shared libraries in the executable directory
            set_target_properties(${target_name} PROPERTIES
                BUILD_RPATH_USE_ORIGIN TRUE
                INSTALL_RPATH "\$ORIGIN"
                BUILD_RPATH "\$ORIGIN"
                SKIP_BUILD_RPATH FALSE
                BUILD_WITH_INSTALL_RPATH FALSE
            )

            # Copy SDL3 shared library
            if(TARGET SDL3-shared)
                add_custom_command(TARGET ${target_name} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    $<TARGET_FILE:SDL3-shared>
                    $<TARGET_FILE_DIR:${target_name}>
                    COMMENT "Copying SDL3 shared library to ${target_name} directory"
                )
            endif()

            # Copy SDL3_image shared library
            if(LAYA_USE_SDL_IMAGE AND TARGET SDL3_image-shared)
                add_custom_command(TARGET ${target_name} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    $<TARGET_FILE:SDL3_image-shared>
                    $<TARGET_FILE_DIR:${target_name}>
                    COMMENT "Copying SDL3_image shared library to ${target_name} directory"
                )
            endif()

            # Copy SDL3_ttf shared library
            if(LAYA_USE_SDL_TTF AND TARGET SDL3_ttf-shared)
                add_custom_command(TARGET ${target_name} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    $<TARGET_FILE:SDL3_ttf-shared>
                    $<TARGET_FILE_DIR:${target_name}>
                    COMMENT "Copying SDL3_ttf shared library to ${target_name} directory"
                )
            endif()

            # On macOS, also set the correct install name and RPATH
            if(APPLE)
                set_target_properties(${target_name} PROPERTIES
                    INSTALL_RPATH "@loader_path"
                    BUILD_WITH_INSTALL_RPATH TRUE
                )
            endif()

            # Add a post-build check for Unix systems to verify dependencies
            add_custom_command(TARGET ${target_name} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E echo "Checking runtime dependencies for ${target_name}..."
                COMMAND bash -c "if command -v ldd >/dev/null 2>&1; then echo 'Dependencies:' && ldd $<TARGET_FILE:${target_name}> | head -10 || echo 'ldd check failed but continuing...'; else echo 'ldd not available, skipping dependency check'; fi"
                COMMENT "Verifying shared library dependencies"
                VERBATIM
            )
        endif()
    endif()
endfunction()
