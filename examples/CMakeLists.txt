# laya examples

add_executable(laya_window_events window_and_events.cpp)
target_link_libraries(laya_window_events PRIVATE laya::laya)

# Copy SDL shared libraries to executable directory for runtime
laya_copy_sdl_shared_libs(laya_window_events)
