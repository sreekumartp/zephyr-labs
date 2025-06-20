cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(sim_water_level_test)

# Include the parent include directory where water_level_sim.h lives
zephyr_include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

# Add the test and implementation source files
target_sources(app PRIVATE
  src/test_sim_water_level.c
  ../src/sim_water_level.c
)
