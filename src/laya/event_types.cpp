#include <stdexcept>
#include <cstring>
#include <string>

#include <laya/events/event_types.hpp>
#include <SDL3/SDL.h>

namespace laya {

namespace {

/// Convert SDL window event type and data to laya types
/// @param sdl_type The SDL event type
/// @param data1 First data field from SDL
/// @param data2 Second data field from SDL
/// @return Pair of window_event_type and window_event_data
std::pair<window_event_type, window_event_data> convert_window_event_data(std::uint32_t sdl_type, std::int32_t data1,
                                                                          std::int32_t data2) {
    switch (sdl_type) {
        case SDL_EVENT_WINDOW_SHOWN:
            return {window_event_type::shown, window_event_data_none{}};

        case SDL_EVENT_WINDOW_HIDDEN:
            return {window_event_type::hidden, window_event_data_none{}};

        case SDL_EVENT_WINDOW_EXPOSED:
            return {window_event_type::exposed, window_event_data_none{}};

        case SDL_EVENT_WINDOW_MOVED:
            return {window_event_type::moved, window_event_data_position{data1, data2}};

        case SDL_EVENT_WINDOW_RESIZED:
            return {window_event_type::resized, window_event_data_size{data1, data2}};

        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
            return {window_event_type::size_changed, window_event_data_size{data1, data2}};

        case SDL_EVENT_WINDOW_MINIMIZED:
            return {window_event_type::minimized, window_event_data_none{}};

        case SDL_EVENT_WINDOW_MAXIMIZED:
            return {window_event_type::maximized, window_event_data_none{}};

        case SDL_EVENT_WINDOW_RESTORED:
            return {window_event_type::restored, window_event_data_none{}};

        case SDL_EVENT_WINDOW_MOUSE_ENTER:
            return {window_event_type::enter, window_event_data_none{}};

        case SDL_EVENT_WINDOW_MOUSE_LEAVE:
            return {window_event_type::leave, window_event_data_none{}};

        case SDL_EVENT_WINDOW_FOCUS_GAINED:
            return {window_event_type::focus_gained, window_event_data_none{}};

        case SDL_EVENT_WINDOW_FOCUS_LOST:
            return {window_event_type::focus_lost, window_event_data_none{}};

        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            return {window_event_type::close, window_event_data_none{}};

        case SDL_EVENT_WINDOW_HIT_TEST:
            return {window_event_type::hit_test, window_event_data_none{}};

        case SDL_EVENT_WINDOW_ICCPROF_CHANGED:
            return {window_event_type::icc_profile_changed, window_event_data_none{}};

        case SDL_EVENT_WINDOW_DISPLAY_CHANGED:
            return {window_event_type::display_changed, window_event_data_display{data1}};

        default:
            throw std::runtime_error("Unknown SDL window event type: " + std::to_string(sdl_type));
    }
}

}  // anonymous namespace

event from_sdl_event(const SDL_Event& sdl_ev) {
    switch (sdl_ev.type) {
        case SDL_EVENT_QUIT: {
            quit_event event;
            event.timestamp = sdl_ev.quit.timestamp;
            return event;
        }

        case SDL_EVENT_WINDOW_SHOWN:
        case SDL_EVENT_WINDOW_HIDDEN:
        case SDL_EVENT_WINDOW_EXPOSED:
        case SDL_EVENT_WINDOW_MOVED:
        case SDL_EVENT_WINDOW_RESIZED:
        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
        case SDL_EVENT_WINDOW_MINIMIZED:
        case SDL_EVENT_WINDOW_MAXIMIZED:
        case SDL_EVENT_WINDOW_RESTORED:
        case SDL_EVENT_WINDOW_MOUSE_ENTER:
        case SDL_EVENT_WINDOW_MOUSE_LEAVE:
        case SDL_EVENT_WINDOW_FOCUS_GAINED:
        case SDL_EVENT_WINDOW_FOCUS_LOST:
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        case SDL_EVENT_WINDOW_HIT_TEST:
        case SDL_EVENT_WINDOW_ICCPROF_CHANGED:
        case SDL_EVENT_WINDOW_DISPLAY_CHANGED: {
            window_event event;
            event.timestamp = sdl_ev.window.timestamp;
            event.id = window_id{sdl_ev.window.windowID};

            auto [type, data] = convert_window_event_data(sdl_ev.type, sdl_ev.window.data1, sdl_ev.window.data2);

            event.event_type = type;
            event.data = data;
            return event;
        }

        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP: {
            key_event event;
            event.timestamp = sdl_ev.key.timestamp;
            event.id = window_id{sdl_ev.key.windowID};
            event.key_state =
                (sdl_ev.type == SDL_EVENT_KEY_DOWN) ? key_event::state::pressed : key_event::state::released;
            event.scancode = static_cast<std::uint32_t>(sdl_ev.key.scancode);
            event.keycode = static_cast<std::uint32_t>(sdl_ev.key.key);
            event.mod = sdl_ev.key.mod;
            event.repeat = sdl_ev.key.repeat != 0;
            return event;
        }

        case SDL_EVENT_TEXT_INPUT: {
            text_input_event event;
            event.timestamp = sdl_ev.text.timestamp;
            event.id = window_id{sdl_ev.text.windowID};
            std::strncpy(event.text, sdl_ev.text.text, sizeof(event.text) - 1);
            event.text[sizeof(event.text) - 1] = '\0';
            return event;
        }

        case SDL_EVENT_TEXT_EDITING: {
            text_editing_event event;
            event.timestamp = sdl_ev.edit.timestamp;
            event.id = window_id{sdl_ev.edit.windowID};
            event.start = sdl_ev.edit.start;
            event.length = sdl_ev.edit.length;
            std::strncpy(event.text, sdl_ev.edit.text, sizeof(event.text) - 1);
            event.text[sizeof(event.text) - 1] = '\0';
            return event;
        }

        case SDL_EVENT_MOUSE_MOTION: {
            mouse_motion_event event;
            event.timestamp = sdl_ev.motion.timestamp;
            event.id = window_id{sdl_ev.motion.windowID};
            event.which = sdl_ev.motion.which;
            event.state = sdl_ev.motion.state;
            event.x = sdl_ev.motion.x;
            event.y = sdl_ev.motion.y;
            event.xrel = sdl_ev.motion.xrel;
            event.yrel = sdl_ev.motion.yrel;
            return event;
        }

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP: {
            mouse_button_event::button btn;
            switch (sdl_ev.button.button) {
                case SDL_BUTTON_LEFT:
                    btn = mouse_button_event::button::left;
                    break;
                case SDL_BUTTON_MIDDLE:
                    btn = mouse_button_event::button::middle;
                    break;
                case SDL_BUTTON_RIGHT:
                    btn = mouse_button_event::button::right;
                    break;
                case SDL_BUTTON_X1:
                    btn = mouse_button_event::button::x1;
                    break;
                case SDL_BUTTON_X2:
                    btn = mouse_button_event::button::x2;
                    break;
                default:
                    btn = mouse_button_event::button::left;
                    break;
            }

            mouse_button_event event;
            event.timestamp = sdl_ev.button.timestamp;
            event.id = window_id{sdl_ev.button.windowID};
            event.which = sdl_ev.button.which;
            event.mouse_button = btn;
            event.button_state = (sdl_ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN) ? mouse_button_event::state::pressed
                                                                              : mouse_button_event::state::released;
            event.clicks = sdl_ev.button.clicks;
            event.x = sdl_ev.button.x;
            event.y = sdl_ev.button.y;
            return event;
        }

        case SDL_EVENT_MOUSE_WHEEL: {
            mouse_wheel_event event;
            event.timestamp = sdl_ev.wheel.timestamp;
            event.id = window_id{sdl_ev.wheel.windowID};
            event.which = sdl_ev.wheel.which;
            event.x = sdl_ev.wheel.x;
            event.y = sdl_ev.wheel.y;
            event.precise_x = static_cast<float>(sdl_ev.wheel.x);
            event.precise_y = static_cast<float>(sdl_ev.wheel.y);
            event.direction = sdl_ev.wheel.direction;
            return event;
        }

        case SDL_EVENT_JOYSTICK_AXIS_MOTION: {
            joystick_axis_event event;
            event.timestamp = sdl_ev.jaxis.timestamp;
            event.which = sdl_ev.jaxis.which;
            event.axis = sdl_ev.jaxis.axis;
            event.value = sdl_ev.jaxis.value;
            return event;
        }

        case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
        case SDL_EVENT_JOYSTICK_BUTTON_UP: {
            joystick_button_event event;
            event.timestamp = sdl_ev.jbutton.timestamp;
            event.which = sdl_ev.jbutton.which;
            event.button = sdl_ev.jbutton.button;
            event.button_state = (sdl_ev.type == SDL_EVENT_JOYSTICK_BUTTON_DOWN)
                                     ? joystick_button_event::state::pressed
                                     : joystick_button_event::state::released;
            return event;
        }

        case SDL_EVENT_JOYSTICK_HAT_MOTION: {
            joystick_hat_event event;
            event.timestamp = sdl_ev.jhat.timestamp;
            event.which = sdl_ev.jhat.which;
            event.hat = sdl_ev.jhat.hat;
            event.value = sdl_ev.jhat.value;
            return event;
        }

        default:
            throw std::runtime_error("Unsupported SDL event type: " + std::to_string(sdl_ev.type));
    }
}

}  // namespace laya
