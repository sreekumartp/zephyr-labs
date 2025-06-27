# This top-level CMakeLists simply makes the subdirectories available.

# add_subdirectory(src)

if(CONFIG_ZTEST)
    add_subdirectory(tests)
endif()