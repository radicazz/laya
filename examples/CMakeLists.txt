# laya examples

add_executable(hello_laya hello_laya.cpp)
target_link_libraries(hello_laya PRIVATE laya::laya)

# Copy SDL shared libraries to executable directory for runtime
laya_copy_sdl_shared_libs(hello_laya)
