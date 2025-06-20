#include <zephyr/ztest.h>
#include "sim_water_level.h"

ZTEST(sim_water_level, test_level_empty_and_full) {
    water_level_sim_set_state(false);
    zassert_false(water_level_sim_get_state(), "Expected water level to be EMPTY");

    water_level_sim_set_state(true);
    zassert_true(water_level_sim_get_state(), "Expected water level to be FULL");
}

ZTEST_SUITE(sim_water_level, NULL, NULL, NULL, NULL, NULL);
