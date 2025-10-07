add_executable(laya_examples_rendering main.cpp)
target_link_libraries(laya_examples_rendering PRIVATE laya::laya)

# Copy SDL shared libraries to executable directory for runtime
laya_copy_sdl_shared_libs(laya_examples_rendering)
