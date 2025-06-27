cmake_minimum_required(VERSION 3.20.0)

# This line is critical and must come first.
list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/../../components/event_bus)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(event_bus_app)

# Add all application sources directly to the 'app' target.
# The 'app' target is automatically created by find_package(Zephyr).
target_sources(app PRIVATE
    src/main.c
    src/sender.c
    src/polling_receiver.c
    src/callback_receiver.c
    src/processing_thread.c
)

# The application needs access to the public headers of the component.
target_include_directories(app PRIVATE
    ../../components/event_bus/include
    src/
)

# Link the application against the library target.
target_link_libraries(app PRIVATE event_bus_lib)