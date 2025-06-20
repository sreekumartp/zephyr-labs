cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(test_sim_fsm)

# Add include directory to find fsm.h
zephyr_include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
)


target_sources(app PRIVATE
  src/test_fsm.c
  ../src/fsm.c
)
