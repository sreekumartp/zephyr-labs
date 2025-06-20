cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(sim_door_sensor_test)

# Include the header from the sim_door_sensor implementation
zephyr_include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

# Link test source and implementation source
target_sources(app PRIVATE
  src/test_sim_door_sensor.c
  ../src/sim_door_sensor.c
)
