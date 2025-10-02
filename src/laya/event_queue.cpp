#include <laya/event_queue.hpp>
#include <SDL3/SDL.h>

namespace laya {

event_range::event_range() {
    SDL_Event sdl_event;
    while (SDL_PollEvent(&sdl_event)) {
        try {
            m_events.push_back(from_sdl_event(sdl_event));
        } catch (const std::runtime_error&) {
            // Skip unsupported event types
            continue;
        }
    }
}

event_range poll_events() {
    return event_range{};
}

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

}  // namespace laya
