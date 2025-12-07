set(LOGGING_EXAMPLES
    logging_basics_levels
    logging_categories_priority
    logging_color_output
    logging_render_loop
)

foreach(example ${LOGGING_EXAMPLES})
    add_executable(laya_examples_${example} ${example}.cpp)
    target_link_libraries(laya_examples_${example} PRIVATE laya::laya laya_examples_common)
    target_compile_features(laya_examples_${example} PRIVATE cxx_std_20)
    laya_copy_sdl_shared_libs(laya_examples_${example})
endforeach()
