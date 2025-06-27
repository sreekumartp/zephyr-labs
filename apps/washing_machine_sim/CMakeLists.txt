cmake_minimum_required(VERSION 3.20.0)
# This line is critical and must come first.
list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/../../components/event_bus)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(washing_machine_sim)

# Add core and simulator source files
target_sources(app PRIVATE
    src/main.c
    src/controller/controller_thread.c
    src/shell_interface.c
    sim_door_sensor/src/sim_door_sensor.c
    sim_water_level/src/sim_water_level.c
)
# Add the FSM module as a subdirectory
add_subdirectory(fsm)

# Add include directory to find fgearders
zephyr_include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
   ${CMAKE_CURRENT_SOURCE_DIR}/../../components/event_bus/include
   ${CMAKE_CURRENT_SOURCE_DIR}/src/controller
)

# Include headers from simulators
target_include_directories(app PRIVATE
    sim_door_sensor/include
    sim_water_level/include
)

# Link the application against the library target.
target_link_libraries(app PRIVATE event_bus_lib)