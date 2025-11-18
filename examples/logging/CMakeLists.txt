# Logging example

add_executable(example_logging main.cpp)

target_link_libraries(example_logging PRIVATE laya::laya)

# Set C++ standard
target_compile_features(example_logging PRIVATE cxx_std_20)
