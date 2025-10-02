/// @file event_polling.hpp
/// @date 2025-10-01
/// @todo Look into not throwing exceptions during event polling.

#pragma once

#include "event_types.hpp"
#include <vector>
#include <optional>
#include <chrono>

namespace laya {

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

/// Range-based container for polling events
/// @note Owns the events - suitable for storing and multi-pass iteration
class event_range {
public:
    /// Constructor that polls all available events
    event_range();

    /// Iterator support for range-based for loops
    [[nodiscard]] auto begin() const noexcept;
    [[nodiscard]] auto end() const noexcept;

    /// Check if any events are available
    [[nodiscard]] bool empty() const noexcept;

    /// Get the number of events
    [[nodiscard]] size_t size() const noexcept;

    /// Access events by index
    [[nodiscard]] const event& operator[](size_t index) const;

private:
    std::vector<event> m_events;
};

/// Poll all available events
/// @return Range containing all polled events (owns events, supports multi-pass iteration)
[[nodiscard]] event_range poll_events_range();

/// Non-owning view for lazy event polling
/// @note Zero-allocation, single-pass iteration - suitable for performance-critical code
class event_view {
public:
    /// Input iterator for lazy event conversion
    class iterator {
    public:
        using value_type = event;
        using difference_type = std::ptrdiff_t;
        using pointer = const event*;
        using reference = const event&;
        using iterator_category = std::input_iterator_tag;

        /// Construct end iterator
        iterator() noexcept;

        /// Dereference operator - returns current event
        [[nodiscard]] const event& operator*() const;

        /// Arrow operator - access current event members
        [[nodiscard]] const event* operator->() const;

        /// Pre-increment - advance to next event
        iterator& operator++();

        /// Post-increment - advance to next event
        iterator operator++(int);

        /// Equality comparison
        [[nodiscard]] bool operator==(const iterator& other) const noexcept;

        /// Inequality comparison
        [[nodiscard]] bool operator!=(const iterator& other) const noexcept;

    private:
        friend class event_view;

        /// Construct begin iterator
        explicit iterator(bool fetch_first);

        /// Fetch next event from SDL
        void fetch_next();

    private:
        event m_current_event;
        bool m_has_event;
    };

    /// Default constructor
    event_view() noexcept = default;

    /// Get begin iterator
    [[nodiscard]] iterator begin() const;

    /// Get end iterator
    [[nodiscard]] iterator end() const noexcept;
};

/// Poll events lazily without allocating storage
/// @return View for zero-allocation, single-pass event iteration
/// @note Prefer this for performance-critical code when events are processed once
[[nodiscard]] event_view poll_events_view() noexcept;

// ============================================================================
// event_range inline implementations
// ============================================================================

inline auto event_range::begin() const noexcept {
    return m_events.begin();
}

inline auto event_range::end() const noexcept {
    return m_events.end();
}

inline bool event_range::empty() const noexcept {
    return m_events.empty();
}

inline size_t event_range::size() const noexcept {
    return m_events.size();
}

inline const event& event_range::operator[](size_t index) const {
    return m_events[index];
}

// ============================================================================
// event_view::iterator inline implementations
// ============================================================================

inline event_view::iterator::iterator() noexcept : m_current_event{}, m_has_event{false} {
}

inline const event& event_view::iterator::operator*() const {
    return m_current_event;
}

inline const event* event_view::iterator::operator->() const {
    return &m_current_event;
}

inline event_view::iterator& event_view::iterator::operator++() {
    fetch_next();
    return *this;
}

inline event_view::iterator event_view::iterator::operator++(int) {
    iterator tmp = *this;
    fetch_next();
    return tmp;
}

inline bool event_view::iterator::operator==(const iterator& other) const noexcept {
    return m_has_event == other.m_has_event;
}

inline bool event_view::iterator::operator!=(const iterator& other) const noexcept {
    return !(*this == other);
}

// ============================================================================
// event_view inline implementations
// ============================================================================

inline event_view::iterator event_view::begin() const {
    return iterator{true};
}

inline event_view::iterator event_view::end() const noexcept {
    return iterator{};
}

inline event_view poll_events_view() noexcept {
    return event_view{};
}

}  // namespace laya
