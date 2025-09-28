cmake_minimum_required(VERSION 3.21)

project(laya
    VERSION 0.1.0
    DESCRIPTION "Modern C++20 wrapper for SDL3"
    LANGUAGES CXX
)

# Check if this is the main project
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(LAYA_IS_ROOT_PROJECT ON)
else()
    set(LAYA_IS_ROOT_PROJECT OFF)
endif()

# Prevent in-source builds when this is the root project
if(LAYA_IS_ROOT_PROJECT AND CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are not allowed. Please create a build directory and run CMake from there.")
endif()

# Project options - conditional defaults based on whether this is the root project
option(LAYA_BUILD_ALL "Build all optional components (tests, examples)" ${LAYA_IS_ROOT_PROJECT})
option(LAYA_BUILD_TESTS "Build laya tests" ${LAYA_BUILD_ALL})
option(LAYA_BUILD_EXAMPLES "Build laya examples" ${LAYA_BUILD_ALL})
option(LAYA_INSTALL "Generate install target" ${LAYA_IS_ROOT_PROJECT})

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
