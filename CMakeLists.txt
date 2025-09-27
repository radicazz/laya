cmake_minimum_required(VERSION 3.21)

project(laya
    VERSION 0.0.1
    DESCRIPTION "Modern C++20 wrapper for SDL3"
    LANGUAGES CXX
)

# Project options
option(LAYA_BUILD_TESTS "Build laya tests" ON)
option(LAYA_BUILD_EXAMPLES "Build laya examples" ON)
option(LAYA_INSTALL "Generate install target" ON)

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
include(cmake/SetupSDL3.cmake)

# Define laya library
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

# Installation
if(LAYA_INSTALL AND NOT LAYA_SDL_METHOD STREQUAL "submodule")
    include(cmake/LayaInstall.cmake)
endif()
