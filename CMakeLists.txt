cmake_minimum_required(VERSION 3.21)

project(laya
    VERSION 0.1.0
    DESCRIPTION "Modern C++20 wrapper for SDL3"
    LANGUAGES CXX
)

# Determine if laya is being consumed as a submodule or as a root project
if(PROJECT_IS_TOP_LEVEL)
    # Prohibit in-source builds
    if(CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
        message(FATAL_ERROR "In-source builds are not allowed. Please create a build directory and run CMake from there.")
    endif()

    # Enable compile commands for tools like clang-tidy
    # Only supported by Ninja and Makefile generators
    if(NOT CMAKE_GENERATOR MATCHES "Visual Studio" AND NOT CMAKE_GENERATOR MATCHES "Xcode")
        set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

        # Copy compile_commands.json to root directory after generation
        add_custom_target(copy_compile_commands ALL
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_BINARY_DIR}/compile_commands.json
            ${CMAKE_SOURCE_DIR}/compile_commands.json
            COMMENT "Copying compile_commands.json to project root"
            VERBATIM
        )
    else()
        message(WARNING
            "Generator '${CMAKE_GENERATOR}' does not support CMAKE_EXPORT_COMPILE_COMMANDS. "
            "compile_commands.json will not be generated. "
            "Consider using Ninja or Makefile generators for clang-tidy support."
        )
    endif()
endif()

# Project options - conditional defaults based on whether this is the root project
option(LAYA_BUILD_ALL "Build all optional components (tests, examples)" ${PROJECT_IS_TOP_LEVEL})
option(LAYA_BUILD_TESTS "Build laya tests" ${LAYA_BUILD_ALL})
option(LAYA_BUILD_EXAMPLES "Build laya examples" ${LAYA_BUILD_ALL})
option(LAYA_INSTALL "Generate install target" ${PROJECT_IS_TOP_LEVEL})

# SDL3 consumption options
option(LAYA_SDL_TARGETS_PROVIDED "Skip SDL setup - parent project provides SDL targets" OFF)
set(LAYA_SDL_METHOD "submodule" CACHE STRING "Method to consume SDL3")
set_property(CACHE LAYA_SDL_METHOD PROPERTY STRINGS "submodule" "system")

# SDL3 extension options
option(LAYA_USE_SDL_IMAGE "Enable SDL3_image support" ON)
option(LAYA_USE_SDL_TTF "Enable SDL3_ttf support" ON)

# C++20 requirement
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add cmake modules path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Include SDL3 setup
include(SetupSDL3)

# Define laya library - always static
add_library(laya STATIC)

add_library(laya::laya ALIAS laya)

# Configure laya target
target_include_directories(laya
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(laya
    PUBLIC
    SDL3::SDL3
    $<$<BOOL:${LAYA_USE_SDL_IMAGE}>:SDL3::SDL3_image>
    $<$<BOOL:${LAYA_USE_SDL_TTF}>:SDL3::SDL3_ttf>
)

target_compile_features(laya PUBLIC cxx_std_20)

# Add subdirectories
add_subdirectory(src)

if(LAYA_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(LAYA_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Installation - only when explicitly requested and not using submodules
# When consumed as a dependency, installation is typically handled by the parent project
if(LAYA_INSTALL AND NOT LAYA_SDL_METHOD STREQUAL "submodule")
    include(cmake/LayaInstall.cmake)
endif()

# Always export targets for use in build tree (allows parent projects to use find_package in build tree)
if(NOT LAYA_SDL_METHOD STREQUAL "submodule")
    export(TARGETS laya
        NAMESPACE laya::
        FILE "${CMAKE_CURRENT_BINARY_DIR}/LayaTargets.cmake"
    )

    # Generate a basic config file for build tree usage
    include(CMakePackageConfigHelpers)
    configure_package_config_file(
        cmake/LayaConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/LayaConfig.cmake
        INSTALL_DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
    )

    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/LayaConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )
endif()
