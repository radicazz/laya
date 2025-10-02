#include <stdexcept>
#include <cstring>
#include <string>

#include <laya/events/event_types.hpp>
#include <SDL3/SDL.h>

namespace laya {

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
            window_event::type event_type;
            switch (sdl_ev.type) {
                case SDL_EVENT_WINDOW_SHOWN:
                    event_type = window_event::type::shown;
                    break;
                case SDL_EVENT_WINDOW_HIDDEN:
                    event_type = window_event::type::hidden;
                    break;
                case SDL_EVENT_WINDOW_EXPOSED:
                    event_type = window_event::type::exposed;
                    break;
                case SDL_EVENT_WINDOW_MOVED:
                    event_type = window_event::type::moved;
                    break;
                case SDL_EVENT_WINDOW_RESIZED:
                    event_type = window_event::type::resized;
                    break;
                case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
                    event_type = window_event::type::size_changed;
                    break;
                case SDL_EVENT_WINDOW_MINIMIZED:
                    event_type = window_event::type::minimized;
                    break;
                case SDL_EVENT_WINDOW_MAXIMIZED:
                    event_type = window_event::type::maximized;
                    break;
                case SDL_EVENT_WINDOW_RESTORED:
                    event_type = window_event::type::restored;
                    break;
                case SDL_EVENT_WINDOW_MOUSE_ENTER:
                    event_type = window_event::type::enter;
                    break;
                case SDL_EVENT_WINDOW_MOUSE_LEAVE:
                    event_type = window_event::type::leave;
                    break;
                case SDL_EVENT_WINDOW_FOCUS_GAINED:
                    event_type = window_event::type::focus_gained;
                    break;
                case SDL_EVENT_WINDOW_FOCUS_LOST:
                    event_type = window_event::type::focus_lost;
                    break;
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                    event_type = window_event::type::close;
                    break;
                case SDL_EVENT_WINDOW_HIT_TEST:
                    event_type = window_event::type::hit_test;
                    break;
                case SDL_EVENT_WINDOW_ICCPROF_CHANGED:
                    event_type = window_event::type::icc_profile_changed;
                    break;
                case SDL_EVENT_WINDOW_DISPLAY_CHANGED:
                    event_type = window_event::type::display_changed;
                    break;
                default:
                    event_type = window_event::type::exposed;
                    break;
            }

            window_event event;
            event.timestamp = sdl_ev.window.timestamp;
            event.id = window_id{sdl_ev.window.windowID};
            event.event_type = event_type;
            event.data1 = sdl_ev.window.data1;
            event.data2 = sdl_ev.window.data2;
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
