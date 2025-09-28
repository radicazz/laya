cmake_minimum_required(VERSION 3.21)

project(laya
    VERSION 0.1.0
    DESCRIPTION "Modern C++20 wrapper for SDL3"
    LANGUAGES CXX
)

# Project options
option(LAYA_BUILD_TESTS "Build laya tests" ON)
option(LAYA_BUILD_EXAMPLES "Build laya examples" ON)
option(LAYA_INSTALL "Generate install target" ON)
option(LAYA_BUILD_SHARED "Build laya as shared library" ON)

# SDL3 consumption options
set(LAYA_SDL_METHOD "submodule" CACHE STRING "Method to consume SDL3")
set_property(CACHE LAYA_SDL_METHOD PROPERTY STRINGS "submodule" "system" "vcpkg")

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

# Define laya library
if(LAYA_BUILD_SHARED)
    add_library(laya SHARED)
else()
    add_library(laya STATIC)
endif()

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

# Configure shared library properties
if(LAYA_BUILD_SHARED)
    # Generate export header for proper symbol visibility on Windows
    include(GenerateExportHeader)
    generate_export_header(laya
        EXPORT_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/include/laya/export.h"
    )

    # Add the generated export header to include directories
    target_include_directories(laya
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    )

    # Set library version properties
    set_target_properties(laya PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        WINDOWS_EXPORT_ALL_SYMBOLS TRUE
    )
endif()

# Add subdirectories
add_subdirectory(src)

if(LAYA_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(LAYA_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Installation
if(LAYA_INSTALL AND NOT LAYA_SDL_METHOD STREQUAL "submodule")
    include(cmake/LayaInstall.cmake)
endif()
