cmake_minimum_required(VERSION 3.20.0)
list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/../../../../components/event_bus)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(sim_door_sensor_test)

# Include the header from the sim_door_sensor implementation
zephyr_include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../../components/event_bus/include
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../fsm/include
)

# Link test source and implementation source
target_sources(app PRIVATE
  src/test_sim_door_sensor.c
  ../src/sim_door_sensor.c
  # ${CMAKE_CURRENT_SOURCE_DIR}/../../../../components/event_bus/src/event_bus.c
)

# Link the application against the library target.
target_link_libraries(app PRIVATE event_bus_lib)