# Basic Usage Examples

This document provides mockups of basic laya usage patterns, demonstrating the intended API design and user experience.

---

## Hello World Example

The simplest possible laya application that creates a window and renders a colored background.

```cpp
#include <laya/laya.hpp>

int main() {
    try {
        // Initialize SDL3 with video subsystem
        laya::context ctx;
        
        // Create a window
        laya::window window("Hello, laya!", {800, 600});
        
        // Create a renderer for the window
        laya::renderer renderer(window);
        
        // Main loop
        bool running = true;
        while (running) {
            // Handle events
            for (const auto& event : laya::poll_events()) {
                if (std::holds_alternative<laya::quit_event>(event)) {
                    running = false;
                }
            }
            
            // Render
            renderer.clear(laya::color::blue());
            renderer.present();
        }
        
        return 0;
    } catch (const laya::error& e) {
        std::cerr << "laya error: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

**Key Features Demonstrated**:

- RAII initialization with `laya::context`
- Simple window creation with title and size
- Automatic renderer creation
- Range-based event polling
- Type-safe event handling with `std::variant`
- Compile-time color constants
- Exception-based error handling

---

## Drawing Primitives

Example showing how to draw basic geometric shapes.

```cpp
#include <laya/laya.hpp>
#include <vector>

int main() {
    laya::context ctx;
    laya::window window("Drawing Primitives", {800, 600});
    laya::renderer renderer(window);
    
    bool running = true;
    while (running) {
        for (const auto& event : laya::poll_events()) {
            if (std::holds_alternative<laya::quit_event>(event)) {
                running = false;
            }
        }
        
        // Clear with dark background
        renderer.clear(laya::color::from_hex(0x2E3440));
        
        // Draw points
        renderer.set_draw_color(laya::color::red());
        std::vector<laya::point> points = {
            {100, 100}, {110, 105}, {120, 110}, {130, 115}
        };
        renderer.draw_points(points);
        
        // Draw lines
        renderer.set_draw_color(laya::color::green());
        renderer.draw_line({200, 100}, {300, 150});
        renderer.draw_line({300, 150}, {250, 200});
        renderer.draw_line({250, 200}, {200, 100});
        
        // Draw rectangles
        renderer.set_draw_color(laya::color::yellow());
        renderer.draw_rect({{400, 100}, {100, 50}});
        
        renderer.set_draw_color(laya::color::cyan());
        renderer.fill_rect({{400, 200}, {100, 50}});
        
        // Draw with custom colors
        renderer.set_draw_color(laya::color::rgba(255, 128, 0, 128));
        renderer.fill_rect({{500, 300}, {80, 80}});
        
        renderer.present();
    }
    
    return 0;
}
```

**Key Features Demonstrated**:

- Drawing individual points and collections of points
- Line drawing with connected segments
- Rectangle outlines and filled rectangles
- Compile-time color creation from hex values
- Runtime color creation with RGBA values
- Template-based drawing functions for containers

---

## Window Management

Example demonstrating window properties and state management.

```cpp
#include <laya/laya.hpp>
#include <chrono>
#include <thread>

int main() {
    laya::context ctx;
    
    // Create window with specific flags
    laya::window window(
        "Window Management Demo", 
        {640, 480}, 
        laya::window_flags::resizable | laya::window_flags::borderless
    );
    
    laya::renderer renderer(window);
    
    // Demonstrate window operations
    window.show();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    window.set_position({100, 100});
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    window.set_size({800, 600});
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    window.set_title("Resized Window!");
    
    bool running = true;
    auto start_time = std::chrono::steady_clock::now();
    
    while (running) {
        for (const auto& event : laya::poll_events()) {
            std::visit([&](const auto& e) {
                using T = std::decay_t<decltype(e)>;
                
                if constexpr (std::is_same_v<T, laya::quit_event>) {
                    running = false;
                } else if constexpr (std::is_same_v<T, laya::key_event>) {
                    if (e.key_state == laya::key_event::state::pressed) {
                        switch (e.scancode) {
                            case SDL_SCANCODE_F:
                                // Toggle fullscreen (mockup - actual implementation would differ)
                                break;
                            case SDL_SCANCODE_M:
                                window.maximize();
                                break;
                            case SDL_SCANCODE_R:
                                window.restore();
                                break;
                            case SDL_SCANCODE_H:
                                window.hide();
                                std::this_thread::sleep_for(std::chrono::seconds(1));
                                window.show();
                                break;
                        }
                    }
                }
            }, event);
        }
        
        // Animate background color based on time
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time);
        
        float t = (elapsed.count() % 3000) / 3000.0f;  // 3-second cycle
        uint8_t intensity = static_cast<uint8_t>(128 + 127 * std::sin(t * 2 * 3.14159f));
        
        renderer.clear(laya::color::rgb(intensity, 64, 128));
        
        // Display window info as colored rectangles (mockup for text)
        auto size = window.size();
        auto pos = window.position();
        
        // Size indicator (width as red intensity, height as green intensity)
        uint8_t red = static_cast<uint8_t>(size.width * 255 / 1920);
        uint8_t green = static_cast<uint8_t>(size.height * 255 / 1080);
        renderer.set_draw_color(laya::color::rgb(red, green, 0));
        renderer.fill_rect({{10, 10}, {100, 50}});
        
        renderer.present();
    }
    
    return 0;
}
```

**Key Features Demonstrated**:

- Window creation with multiple flags using bitwise OR
- Window property modification (position, size, title)
- Window state management (show, hide, maximize, restore)
- Advanced event handling with `std::visit`
- Key event processing with scancode handling
- Window property queries
- Time-based animations

---

## Multiple Subsystems

Example showing initialization of multiple SDL subsystems.

```cpp
#include <laya/laya.hpp>
#include <iostream>

int main() {
    try {
        // Initialize multiple subsystems
        laya::context ctx{
            laya::subsystem::video,
            laya::subsystem::audio,
            laya::subsystem::gamepad,
            laya::subsystem::events
        };
        
        std::cout << "Initialized subsystems:\n";
        auto systems = ctx.initialized_subsystems();
        
        if (laya::has_subsystem(systems, laya::subsystem::video)) {
            std::cout << "  - Video\n";
        }
        if (laya::has_subsystem(systems, laya::subsystem::audio)) {
            std::cout << "  - Audio\n";
        }
        if (laya::has_subsystem(systems, laya::subsystem::gamepad)) {
            std::cout << "  - Gamepad\n";
        }
        if (laya::has_subsystem(systems, laya::subsystem::events)) {
            std::cout << "  - Events\n";
        }
        
        // Runtime subsystem management
        if (!ctx.is_initialized(laya::subsystem::haptic)) {
            std::cout << "Initializing haptic subsystem...\n";
            ctx.init_subsystem(laya::subsystem::haptic);
        }
        
        laya::window window("Multi-Subsystem Demo", {800, 600});
        laya::renderer renderer(window);
        
        // Display SDL version info
        auto version = laya::context::version();
        std::cout << "SDL Version: " << version.major << "." 
                  << version.minor << "." << version.patch << std::endl;
        
        bool running = true;
        while (running) {
            for (const auto& event : laya::poll_events()) {
                std::visit([&](const auto& e) {
                    using T = std::decay_t<decltype(e)>;
                    
                    if constexpr (std::is_same_v<T, laya::quit_event>) {
                        running = false;
                    } else if constexpr (std::is_same_v<T, laya::gamepad_button_event>) {
                        std::cout << "Gamepad button event detected\n";
                    } else if constexpr (std::is_same_v<T, laya::audio_device_event>) {
                        std::cout << "Audio device event detected\n";
                    }
                }, event);
            }
            
            renderer.clear(laya::color::black());
            
            // Visual indicator of active subsystems
            int y = 50;
            if (ctx.is_initialized(laya::subsystem::video)) {
                renderer.set_draw_color(laya::color::green());
                renderer.fill_rect({{50, y}, {100, 30}});
                y += 40;
            }
            if (ctx.is_initialized(laya::subsystem::audio)) {
                renderer.set_draw_color(laya::color::blue());
                renderer.fill_rect({{50, y}, {100, 30}});
                y += 40;
            }
            if (ctx.is_initialized(laya::subsystem::gamepad)) {
                renderer.set_draw_color(laya::color::red());
                renderer.fill_rect({{50, y}, {100, 30}});
                y += 40;
            }
            if (ctx.is_initialized(laya::subsystem::haptic)) {
                renderer.set_draw_color(laya::color::yellow());
                renderer.fill_rect({{50, y}, {100, 30}});
                y += 40;
            }
            
            renderer.present();
        }
        
        return 0;
    } catch (const laya::error& e) {
        std::cerr << "laya error: " << e.what() << std::endl;
        return 1;
    }
}
```

**Key Features Demonstrated**:

- Multi-subsystem initialization with initializer list
- Subsystem query and validation functions
- Runtime subsystem management
- Version information queries
- Extended event handling for different subsystem events
- Visual feedback for subsystem status

---

## Error Handling Patterns

Example demonstrating different error handling approaches.

```cpp
#include <laya/laya.hpp>
#include <iostream>
#include <optional>

// Helper function demonstrating std::expected pattern (future feature)
std::optional<laya::window> try_create_window(const std::string& title, laya::size sz) {
    try {
        return laya::window(title, sz);
    } catch (const laya::error&) {
        return std::nullopt;
    }
}

int main() {
    // Pattern 1: Exception handling for critical operations
    try {
        laya::context ctx;
        
        // Pattern 2: Optional for operations that might fail
        auto window_opt = try_create_window("Error Handling Demo", {800, 600});
        if (!window_opt) {
            std::cerr << "Failed to create window\n";
            return 1;
        }
        
        laya::window& window = *window_opt;
        laya::renderer renderer(window);
        
        bool running = true;
        while (running) {
            for (const auto& event : laya::poll_events()) {
                if (std::holds_alternative<laya::quit_event>(event)) {
                    running = false;
                }
            }
            
            // Pattern 3: Exception handling for rendering operations
            try {
                renderer.clear(laya::color::green());
                
                // Simulate potential error condition
                static int frame_count = 0;
                if (++frame_count == 100) {
                    // This would normally be a real error condition
                    throw laya::error("Simulated rendering error");
                }
                
                renderer.present();
            } catch (const laya::error& e) {
                std::cerr << "Rendering error: " << e.what() << std::endl;
                // Continue running despite rendering error
                frame_count = 0;  // Reset to avoid repeated errors
            }
        }
        
        return 0;
        
    } catch (const laya::error& e) {
        std::cerr << "Fatal laya error: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}
```

**Key Features Demonstrated**:

- Exception handling for critical initialization
- Optional pattern for operations that might fail
- Graceful error recovery in render loop
- Different error handling strategies for different scenarios

---

## Resource Management

Example demonstrating RAII and resource lifecycle management.

```cpp
#include <laya/laya.hpp>
#include <memory>
#include <vector>

class game_object {
public:
    game_object(laya::point pos, laya::color col) 
        : m_position(pos), m_color(col) {}
    
    void draw(laya::renderer& renderer) const {
        renderer.set_draw_color(m_color);
        renderer.fill_rect({m_position, {20, 20}});
    }
    
    void move(laya::point delta) {
        m_position.x += delta.x;
        m_position.y += delta.y;
    }
    
private:
    laya::point m_position;
    laya::color m_color;
};

int main() {
    // RAII context - automatically manages SDL lifetime
    laya::context ctx;
    
    // RAII window - automatically destroyed when scope ends
    laya::window window("Resource Management Demo", {800, 600});
    
    // RAII renderer - tied to window lifetime
    laya::renderer renderer(window);
    
    // Container of game objects - automatic cleanup
    std::vector<std::unique_ptr<game_object>> objects;
    
    // Create some objects
    for (int i = 0; i < 10; ++i) {
        objects.push_back(std::make_unique<game_object>(
            laya::point{i * 50 + 100, i * 30 + 100},
            laya::color::rgb(255 - i * 25, i * 25, 128)
        ));
    }
    
    bool running = true;
    while (running) {
        for (const auto& event : laya::poll_events()) {
            std::visit([&](const auto& e) {
                using T = std::decay_t<decltype(e)>;
                
                if constexpr (std::is_same_v<T, laya::quit_event>) {
                    running = false;
                } else if constexpr (std::is_same_v<T, laya::key_event>) {
                    if (e.key_state == laya::key_event::state::pressed) {
                        // Move all objects
                        laya::point delta{0, 0};
                        switch (e.scancode) {
                            case SDL_SCANCODE_UP:    delta = {0, -5}; break;
                            case SDL_SCANCODE_DOWN:  delta = {0, 5}; break;
                            case SDL_SCANCODE_LEFT:  delta = {-5, 0}; break;
                            case SDL_SCANCODE_RIGHT: delta = {5, 0}; break;
                        }
                        
                        for (auto& obj : objects) {
                            obj->move(delta);
                        }
                    }
                }
            }, event);
        }
        
        renderer.clear(laya::color::black());
        
        // Draw all objects
        for (const auto& obj : objects) {
            obj->draw(renderer);
        }
        
        renderer.present();
    }
    
    // All resources automatically cleaned up:
    // - objects vector destroys all game_objects
    // - renderer destroyed when leaving scope
    // - window destroyed when leaving scope  
    // - context destroyed, SDL quit automatically
    
    return 0;
}
```

**Key Features Demonstrated**:

- RAII resource management throughout
- Automatic cleanup of all resources
- Container management with smart pointers
- Object-oriented design with laya integration
- No manual resource cleanup required

---

## Performance Patterns

Example showing performance-conscious usage patterns.

```cpp
#include <laya/laya.hpp>
#include <chrono>
#include <vector>
#include <random>

class particle {
public:
    laya::point position;
    laya::point velocity;
    laya::color color;
    float life;
    
    particle(laya::point pos, laya::point vel, laya::color col) 
        : position(pos), velocity(vel), color(col), life(1.0f) {}
    
    void update(float dt) {
        position.x += static_cast<int>(velocity.x * dt);
        position.y += static_cast<int>(velocity.y * dt);
        life -= dt;
    }
    
    bool is_alive() const { return life > 0.0f; }
};

int main() {
    laya::context ctx;
    laya::window window("Performance Demo", {1024, 768});
    laya::renderer renderer(window);
    
    std::vector<particle> particles;
    particles.reserve(10000);  // Pre-allocate for performance
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> pos_dist(0, 1024);
    std::uniform_int_distribution<> vel_dist(-100, 100);
    std::uniform_int_distribution<> color_dist(0, 255);
    
    auto last_time = std::chrono::high_resolution_clock::now();
    
    bool running = true;
    while (running) {
        auto current_time = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float>(current_time - last_time).count();
        last_time = current_time;
        
        for (const auto& event : laya::poll_events()) {
            std::visit([&](const auto& e) {
                using T = std::decay_t<decltype(e)>;
                
                if constexpr (std::is_same_v<T, laya::quit_event>) {
                    running = false;
                } else if constexpr (std::is_same_v<T, laya::mouse_button_event>) {
                    if (e.button_state == laya::mouse_button_event::state::pressed) {
                        // Spawn particles at mouse position
                        for (int i = 0; i < 50; ++i) {
                            particles.emplace_back(
                                e.position,
                                laya::point{vel_dist(gen), vel_dist(gen)},
                                laya::color::rgb(color_dist(gen), color_dist(gen), color_dist(gen))
                            );
                        }
                    }
                }
            }, event);
        }
        
        // Update particles (remove dead ones efficiently)
        particles.erase(
            std::remove_if(particles.begin(), particles.end(),
                [dt](particle& p) {
                    p.update(dt);
                    return !p.is_alive();
                }),
            particles.end()
        );
        
        renderer.clear(laya::color::black());
        
        // Batch drawing by color for performance
        std::sort(particles.begin(), particles.end(),
            [](const particle& a, const particle& b) {
                return std::tie(a.color.r, a.color.g, a.color.b) < 
                       std::tie(b.color.r, b.color.g, b.color.b);
            });
        
        laya::color current_color{0, 0, 0, 0};
        std::vector<laya::point> batch_points;
        batch_points.reserve(1000);
        
        for (const auto& p : particles) {
            if (p.color.r != current_color.r || 
                p.color.g != current_color.g || 
                p.color.b != current_color.b) {
                
                // Draw current batch
                if (!batch_points.empty()) {
                    renderer.set_draw_color(current_color);
                    renderer.draw_points(batch_points);
                    batch_points.clear();
                }
                
                current_color = p.color;
            }
            
            batch_points.push_back(p.position);
        }
        
        // Draw final batch
        if (!batch_points.empty()) {
            renderer.set_draw_color(current_color);
            renderer.draw_points(batch_points);
        }
        
        renderer.present();
    }
    
    return 0;
}
```

**Key Features Demonstrated**:

- Pre-allocation for performance
- Efficient container operations (erase-remove idiom)
- Batch rendering to minimize state changes
- Time-based animation with delta time
- Memory-efficient particle system
- Template-based batch drawing functions

---

## Summary

These examples demonstrate laya's design principles in action:

1. **RAII Everywhere**: Automatic resource management eliminates manual cleanup
2. **Type Safety**: Strong types prevent common errors at compile time
3. **Modern C++**: Leverages C++20 features for better APIs
4. **Zero Overhead**: Performance matches raw SDL3 usage
5. **Intuitive APIs**: Natural C++ patterns and STL-like interfaces
6. **Exception Safety**: Proper error handling with exceptions and alternatives
7. **Flexibility**: Easy to integrate with existing C++ codebases

The API design prioritizes safety and ergonomics while maintaining the performance and flexibility that makes SDL3 popular for multimedia applications.
