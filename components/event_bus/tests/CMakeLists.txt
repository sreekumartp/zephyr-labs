# This script builds the ZTest application.
cmake_minimum_required(VERSION 3.20.0)
# This line is critical and must come first.
list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/../../event_bus)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(event_bus_ztest)


# The test needs access to the component's public headers.
target_include_directories(app PRIVATE
    ../../include
 )



target_sources(app PRIVATE
   src/test_event_bus.c
)


# Add the test application source file.
# zephyr_sources(
#     src/test_event_bus.c
# )

# Link the test application against the component's library.
target_link_libraries(app PRIVATE event_bus_lib)