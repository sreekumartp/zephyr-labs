cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(door_sensor)


# Add your application's source files to the 'app' target
target_sources(app PRIVATE src/door_sensor.c)
target_include_directories(app PRIVATE
    include
    ../sim_door_sensor/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../components/event_bus/include
)

# zephyr_library_named(door_sensor)
# zephyr_library_sources(
#     src/door_sensor.c
# )
# zephyr_include_directories(
#     include
#     ../sim_door_sensor/include
# )