cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(washing_machine_sim)

# Add core and simulator source files
target_sources(app PRIVATE
    src/main.c
    src/fsm.c
    src/shell_interface.c
    sim_door_sensor/src/sim_door_sensor.c
    sim_water_level/src/sim_water_level.c
)
# Add include directory to find fgearders
zephyr_include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Include headers from simulators
target_include_directories(app PRIVATE
    sim_door_sensor/include
    sim_water_level/include
)

