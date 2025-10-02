#include <laya/events/event_polling.hpp>
#include <SDL3/SDL.h>

namespace laya {

// ============================================================================
// Event utilities
// ============================================================================

std::optional<event> wait_event() {
    SDL_Event sdl_event;
    if (SDL_WaitEvent(&sdl_event)) {
        try {
            return from_sdl_event(sdl_event);
        } catch (const std::runtime_error&) {
            // If we get an unsupported event, try again
            return wait_event();
        }
    }
    return std::nullopt;
}

std::optional<event> wait_event_timeout(std::chrono::milliseconds timeout) {
    SDL_Event sdl_event;
    if (SDL_WaitEventTimeout(&sdl_event, static_cast<int32_t>(timeout.count()))) {
        try {
            return from_sdl_event(sdl_event);
        } catch (const std::runtime_error&) {
            // If we get an unsupported event, return nullopt for timeout case
            return std::nullopt;
        }
    }
    return std::nullopt;
}

bool has_events() {
    return SDL_PollEvent(nullptr) == true;
}

void flush_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        // Discard all events
    }
}

void flush_events(uint32_t min_type, uint32_t max_type) {
    SDL_FlushEvents(min_type, max_type);
}

// ============================================================================
// event_range implementation
// ============================================================================

event_range::event_range() {
    SDL_Event sdl_event;
    while (SDL_PollEvent(&sdl_event)) {
        try {
            m_events.emplace_back(from_sdl_event(sdl_event));
        } catch (const std::runtime_error&) {
            // Skip unsupported event types
            continue;
        }
    }
}

event_range poll_events_range() {
    return event_range{};
}

// ============================================================================
// event_view::iterator implementation
// ============================================================================

event_view::iterator::iterator(bool fetch_first) : m_current_event{}, m_has_event{false} {
    if (fetch_first) {
        fetch_next();
    }
}

void event_view::iterator::fetch_next() {
    SDL_Event sdl_event;
    while (SDL_PollEvent(&sdl_event)) {
        try {
            m_current_event = from_sdl_event(sdl_event);
            m_has_event = true;
            return;
        } catch (const std::runtime_error&) {
            // Skip unsupported event types and try next
            continue;
        }
    }
    // No more events
    m_has_event = false;
}

}  // namespace laya
