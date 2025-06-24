# CMakeLists.txt for the L1 FSM test application

# Standard Zephyr project setup
cmake_minimum_required(VERSION 3.22)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(l1_fsm_test)

# Add the source files to be compiled for this test application.
# This includes the Ztest source file itself and the L1 FSM implementation
# which is the code being tested.
target_sources(app PRIVATE
    src/test_l1_system_fsm.c
    ../src/l1_system_fsm.c
    src/test_l2_wash_cycle_fsm.c
    ../src/l2_wash_cycle_fsm.c    
    )

# The test requires the parent FSM include directory to find the header files
# (fsm.h, l1_system_fsm.h, etc.).
target_include_directories(app PRIVATE
    ../include
    ../src
    )

