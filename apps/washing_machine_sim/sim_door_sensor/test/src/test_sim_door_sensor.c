#include <zephyr/ztest.h>
#include "sim_door_sensor.h"

ZTEST(sim_door_sensor, test_open_and_close) {
    door_sensor_sim_set_state(false);
    zassert_false(door_sensor_sim_get_state(), "Expected door to be open");

    door_sensor_sim_set_state(true);
    zassert_true(door_sensor_sim_get_state(), "Expected door to be closed");
}

ZTEST_SUITE(sim_door_sensor, NULL, NULL, NULL, NULL, NULL);
