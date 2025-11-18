#include <laya/logging/log.hpp>

#include <cstdio>
#include <format>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <SDL3/SDL.h>

namespace laya {

// ============================================================================
// Internal conversion functions
// ============================================================================

namespace {

constexpr int to_sdl_category(log_category cat) noexcept {
    switch (cat) {
        case log_category::application:
            return SDL_LOG_CATEGORY_APPLICATION;
        case log_category::error:
            return SDL_LOG_CATEGORY_ERROR;
        case log_category::assert_cat:
            return SDL_LOG_CATEGORY_ASSERT;
        case log_category::system:
            return SDL_LOG_CATEGORY_SYSTEM;
        case log_category::audio:
            return SDL_LOG_CATEGORY_AUDIO;
        case log_category::video:
            return SDL_LOG_CATEGORY_VIDEO;
        case log_category::render:
            return SDL_LOG_CATEGORY_RENDER;
        case log_category::input:
            return SDL_LOG_CATEGORY_INPUT;
        case log_category::test:
            return SDL_LOG_CATEGORY_TEST;
        case log_category::gpu:
            return SDL_LOG_CATEGORY_GPU;
        case log_category::custom:
            return SDL_LOG_CATEGORY_CUSTOM;
    }
    return SDL_LOG_CATEGORY_APPLICATION;
}

constexpr SDL_LogPriority to_sdl_priority(log_priority pri) noexcept {
    switch (pri) {
        case log_priority::trace:
            return SDL_LOG_PRIORITY_TRACE;
        case log_priority::verbose:
            return SDL_LOG_PRIORITY_VERBOSE;
        case log_priority::debug:
            return SDL_LOG_PRIORITY_DEBUG;
        case log_priority::info:
            return SDL_LOG_PRIORITY_INFO;
        case log_priority::warn:
            return SDL_LOG_PRIORITY_WARN;
        case log_priority::error:
            return SDL_LOG_PRIORITY_ERROR;
        case log_priority::critical:
            return SDL_LOG_PRIORITY_CRITICAL;
    }
    return SDL_LOG_PRIORITY_INFO;
}

constexpr log_priority from_sdl_priority(SDL_LogPriority pri) noexcept {
    switch (pri) {
        case SDL_LOG_PRIORITY_TRACE:
            return log_priority::trace;
        case SDL_LOG_PRIORITY_VERBOSE:
            return log_priority::verbose;
        case SDL_LOG_PRIORITY_DEBUG:
            return log_priority::debug;
        case SDL_LOG_PRIORITY_INFO:
            return log_priority::info;
        case SDL_LOG_PRIORITY_WARN:
            return log_priority::warn;
        case SDL_LOG_PRIORITY_ERROR:
            return log_priority::error;
        case SDL_LOG_PRIORITY_CRITICAL:
            return log_priority::critical;
        default:
            return log_priority::info;
    }
}

// State for color support
bool g_colors_enabled = false;

// Custom output function with color support
void color_log_output(void* userdata, int category, SDL_LogPriority priority, const char* message) {
    (void)userdata;
    (void)category;

    if (!g_colors_enabled) {
        SDL_GetDefaultLogOutputFunction()(nullptr, category, priority, message);
        return;
    }

    // ANSI color codes (work on Linux/Mac and modern Windows terminals)
    const char* color_code = "";
    const char* reset_code = "\033[0m";

    switch (priority) {
        case SDL_LOG_PRIORITY_TRACE:
            color_code = "\033[2;37m";  // Dim white
            break;
        case SDL_LOG_PRIORITY_VERBOSE:
            color_code = "\033[37m";  // White
            break;
        case SDL_LOG_PRIORITY_DEBUG:
            color_code = "\033[36m";  // Cyan
            break;
        case SDL_LOG_PRIORITY_INFO:
            color_code = "\033[32m";  // Green
            break;
        case SDL_LOG_PRIORITY_WARN:
            color_code = "\033[33m";  // Yellow
            break;
        case SDL_LOG_PRIORITY_ERROR:
            color_code = "\033[31m";  // Red
            break;
        case SDL_LOG_PRIORITY_CRITICAL:
            color_code = "\033[1;31m";  // Bold red
            break;
        default:
            color_code = "";
            reset_code = "";
            break;
    }

    // Print with color if enabled
    std::fprintf(stderr, "%s%s%s\n", color_code, message, reset_code);
}

}  // namespace

// ============================================================================
// Detail namespace implementation
// ============================================================================

namespace detail {

void log_message(log_category category, log_priority priority, const char* message) {
    SDL_LogMessage(to_sdl_category(category), to_sdl_priority(priority), "%s", message);
}

void log_message_with_location(log_category category, log_priority priority, const char* message,
                               const std::source_location& loc) {
    const auto formatted = std::format("[{}:{}] {}", loc.file_name(), loc.line(), message);
    SDL_LogMessage(to_sdl_category(category), to_sdl_priority(priority), "%s", formatted.c_str());
}

}  // namespace detail

// ============================================================================
// Priority management
// ============================================================================

void set_log_priority(log_category category, log_priority priority) {
    SDL_SetLogPriority(to_sdl_category(category), to_sdl_priority(priority));
}

log_priority get_log_priority(log_category category) {
    return from_sdl_priority(SDL_GetLogPriority(to_sdl_category(category)));
}

void set_all_log_priorities(log_priority priority) {
    SDL_SetLogPriorities(to_sdl_priority(priority));
}

void reset_log_priorities() noexcept {
    SDL_ResetLogPriorities();
}

// ============================================================================
// Console color support
// ============================================================================

bool enable_log_colors() noexcept {
    // Check if terminal supports colors
    // On Linux/Mac: check if stderr is a TTY
    // On Windows: Windows 10+ supports ANSI escape codes
#ifdef _WIN32
// Enable ANSI escape code support on Windows 10+
// This is a no-op on older Windows versions
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

    HANDLE hConsole = GetStdHandle(STD_ERROR_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD mode = 0;
    if (!GetConsoleMode(hConsole, &mode)) {
        return false;
    }

    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hConsole, mode)) {
        return false;
    }
#else
    // On Unix-like systems, check if stderr is a TTY
    if (!isatty(fileno(stderr))) {
        return false;
    }
#endif

    g_colors_enabled = true;
    SDL_SetLogOutputFunction(color_log_output, nullptr);
    return true;
}

void disable_log_colors() noexcept {
    g_colors_enabled = false;
    SDL_SetLogOutputFunction(nullptr, nullptr);  // Reset to default
}

bool are_log_colors_enabled() noexcept {
    return g_colors_enabled;
}

// ============================================================================
// RAII priority guard
// ============================================================================

log_priority_guard::log_priority_guard(log_category category, log_priority new_priority)
    : m_category(category), m_old_priority(get_log_priority(category)) {
    set_log_priority(category, new_priority);
}

log_priority_guard::~log_priority_guard() noexcept {
    set_log_priority(m_category, m_old_priority);
}

}  // namespace laya
