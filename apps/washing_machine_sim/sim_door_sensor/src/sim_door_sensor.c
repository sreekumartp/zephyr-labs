#include "sim_door_sensor.h"

static bool door_closed = false;

void door_sensor_sim_set_state(bool closed) {
    door_closed = closed;
}

bool door_sensor_sim_get_state(void) {
    return door_closed;
}
