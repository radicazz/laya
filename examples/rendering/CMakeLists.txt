set(RENDERING_EXAMPLES
    render_clear_screen
    render_draw_primitives
    render_quit_with_events
)

foreach(example ${RENDERING_EXAMPLES})
    add_executable(laya_examples_${example} ${example}.cpp)
    target_link_libraries(laya_examples_${example} PRIVATE laya::laya laya_examples_common)
    target_compile_features(laya_examples_${example} PRIVATE cxx_std_20)

    # Copy SDL shared libraries to executable directory for runtime
    laya_copy_sdl_shared_libs(laya_examples_${example})
endforeach()
