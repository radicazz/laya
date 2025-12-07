set(EVENT_EXAMPLES
    event_view_basic
    event_range_multipass
    window_event_data
    quit_and_escape
)

foreach(example ${EVENT_EXAMPLES})
    add_executable(laya_examples_${example} ${example}.cpp)
    target_link_libraries(laya_examples_${example} PRIVATE laya::laya laya_examples_common)
    target_compile_features(laya_examples_${example} PRIVATE cxx_std_20)

    # Copy SDL shared libraries to executable directory for runtime
    laya_copy_sdl_shared_libs(laya_examples_${example})
endforeach()
