#include "sim_water_level.h"

static bool water_full = false;

void water_level_sim_set_state(bool full) {
    water_full = full;
}

bool water_level_sim_get_state(void) {
    return water_full;
}
