/// @file event_queue.hpp
/// @date 2025-10-01

#pragma once

#include "events.hpp"
#include <vector>
#include <optional>
#include <chrono>

namespace laya {

/// Range-based container for polling events
class event_range {
public:
    /// Constructor that polls all available events
    event_range();

    /// Iterator support for range-based for loops
    auto begin() const {
        return m_events.begin();
    }
    auto end() const {
        return m_events.end();
    }

    /// Check if any events are available
    bool empty() const {
        return m_events.empty();
    }

    /// Get the number of events
    size_t size() const {
        return m_events.size();
    }

    /// Access events by index
    const event& operator[](size_t index) const {
        return m_events[index];
    }

private:
    std::vector<event> m_events;
};

/// Poll all available events
/// @return Range containing all polled events
event_range poll_events();

/// Wait for a single event
/// @return The next event, or nullopt if there was an error
std::optional<event> wait_event();

/// Wait for a single event with timeout
/// @param timeout Maximum time to wait for an event
/// @return The next event, or nullopt if timeout occurred or there was an error
std::optional<event> wait_event_timeout(std::chrono::milliseconds timeout);

/// Check if there are any events in the queue without removing them
/// @return True if there are events available
bool has_events();

/// Flush all events from the queue
void flush_events();

/// Flush specific event types from the queue
/// @param min_type Minimum event type to flush (SDL event type)
/// @param max_type Maximum event type to flush (SDL event type)
void flush_events(uint32_t min_type, uint32_t max_type);

}  // namespace laya
