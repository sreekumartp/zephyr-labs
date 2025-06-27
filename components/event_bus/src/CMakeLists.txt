# # This file now defines a standard CMake library target.
# # It should not contain any "zephyr_" functions.

# add_library(event_bus_lib event_bus.c)

# # This makes the public include directory available to any target that
# # links against event_bus_lib.
# target_include_directories(event_bus_lib
#   PUBLIC
#     ../include
# )


# This is the correct and final version for the component's CMakeLists.txt

# Use Zephyr's function to define the library.
zephyr_library_named(event_bus_lib)

# Add the library's source code.
zephyr_library_sources(event_bus.c)

# Make the public headers available to any target that links this library.
zephyr_library_include_directories(../include)

# This line is essential for standalone app builds and does not harm Twister builds.
# zephyr_library_kconfig(Kconfig)