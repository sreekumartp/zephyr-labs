cmake_minimum_required(VERSION 3.20.0)
set(BOARD native_sim)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(my_timer_app)
target_sources(app PRIVATE src/main.c)

