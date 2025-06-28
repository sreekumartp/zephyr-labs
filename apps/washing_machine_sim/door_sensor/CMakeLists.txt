zephyr_library_named(door_sensor door_sensor)
zephyr_library_sources(
    src/door_sensor.c
)
zephyr_include_directories(
    include
    ../sim_door_sensor/include
)