# SDL3 setup with FetchContent fallback
# Resolution order:
# 1. Parent-provided targets (LAYA_SDL_TARGETS_PROVIDED)
# 2. System-installed SDL3 (find_package)
# 3. FetchContent automatic download

include(FetchContent)

# Option 1: Skip SDL setup if parent project provides targets
if(LAYA_SDL_TARGETS_PROVIDED)
    # Verify that required target exists
    if(NOT TARGET SDL3::SDL3)
        message(FATAL_ERROR "LAYA_SDL_TARGETS_PROVIDED is ON but SDL3::SDL3 target not found")
    endif()

    message(STATUS "Laya: Using SDL3 target provided by parent project")
    return()
endif()

# Option 2: Try to find system-installed SDL3
find_package(SDL3 QUIET)

if(SDL3_FOUND)
    message(STATUS "Laya: Found system SDL3 version ${SDL3_VERSION}")
else()
    message(STATUS "Laya: System SDL3 not found, using FetchContent")
endif()

# Option 3: Use FetchContent to download and build SDL3 if not found
if(NOT SDL3_FOUND)
    message(STATUS "Laya: Fetching SDL3 from GitHub...")
    
    # Configure SDL3 options before fetching
    set(SDL_SHARED ON CACHE BOOL "Build SDL3 as shared library" FORCE)
    set(SDL_STATIC OFF CACHE BOOL "Build SDL3 as static library" FORCE)
    set(SDL_TEST OFF CACHE BOOL "Build SDL3 tests" FORCE)
    
    FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.2.6
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )
    
    FetchContent_MakeAvailable(SDL3)
    
    # Create namespace aliases that match find_package() behavior
    if(NOT TARGET SDL3::SDL3 AND TARGET SDL3-shared)
        add_library(SDL3::SDL3 ALIAS SDL3-shared)
    endif()
    
    if(NOT TARGET SDL3::Headers AND TARGET SDL3-headers)
        add_library(SDL3::Headers ALIAS SDL3-headers)
    endif()
    
    message(STATUS "Laya: SDL3 fetched and configured successfully")
endif()

# Verify required target exists
if(NOT TARGET SDL3::SDL3)
    message(FATAL_ERROR "SDL3::SDL3 target not found after all resolution attempts")
endif()

# Function to copy SDL shared libraries to executable directory and set RPATH
function(laya_copy_sdl_shared_libs target_name)
    # Only copy libraries if we built them via FetchContent
    if(NOT SDL3_FOUND)
        # On Windows, copy DLLs to executable directory
        if(WIN32)
            if(TARGET SDL3-shared)
                add_custom_command(TARGET ${target_name} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    $<TARGET_FILE:SDL3-shared>
                    $<TARGET_FILE_DIR:${target_name}>
                    COMMENT "Copying SDL3 shared library to ${target_name} directory"
                )
            endif()

        # On Unix-like systems (Linux/macOS), set RPATH and copy libraries
        else()
            # Set RPATH to look for shared libraries in the executable directory
            set_target_properties(${target_name} PROPERTIES
                BUILD_RPATH_USE_ORIGIN TRUE
                INSTALL_RPATH "$ORIGIN"
                BUILD_RPATH "$ORIGIN"
                SKIP_BUILD_RPATH FALSE
                BUILD_WITH_INSTALL_RPATH FALSE
            )

            if(TARGET SDL3-shared)
                add_custom_command(TARGET ${target_name} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    $<TARGET_FILE:SDL3-shared>
                    $<TARGET_FILE_DIR:${target_name}>
                    COMMENT "Copying SDL3 shared library to ${target_name} directory"
                )
            endif()

            # On macOS, also set the correct install name and RPATH
            if(APPLE)
                set_target_properties(${target_name} PROPERTIES
                    INSTALL_RPATH "@loader_path"
                    BUILD_WITH_INSTALL_RPATH TRUE
                )
            endif()
        endif()
    endif()
endfunction()
