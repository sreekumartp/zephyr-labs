# Add the public 'include' directory and the private 'src' directory
# to the parent 'app' target's include paths. This allows the main app
# to use `#include "fsm.h"` and allows the internal FSM files to
# include each other (e.g., `#include "l1_system_fsm.h"`).
target_include_directories(app PRIVATE
    include
    src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../components/event_bus/include
)

# Adds the FSM source files and include paths to the main application target.
# This file should be included from the parent application's CMakeLists.txt
# using the add_subdirectory() command.

# Add all C source files in the 'src' subdirectory to the parent 'app' target.
target_sources(app PRIVATE
    src/fsm.c
    src/l1_system_fsm.c
    src/l2_wash_cycle_fsm.c
)

