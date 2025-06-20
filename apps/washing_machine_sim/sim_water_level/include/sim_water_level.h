#pragma once
#include <stdbool.h>

// Set whether water level is full (true) or not (false)
void water_level_sim_set_state(bool full);

// Get current water level state
bool water_level_sim_get_state(void);
