#include <zephyr/ztest.h>
#include "sim_door_sensor.h"
#include "fsm.h"



ZTEST(wm_test_suite, test_wash_cycle_door_open)
{
    // Simulate door open during wash cycle
    door_sensor_sim_set_state(false);  // Simulate door open
    zassert_false(is_door_closed(), "WM should detect door is open");
}

ZTEST(wm_test_suite, test_wash_cycle_door_closed)
{
    // Simulate door closed during wash cycle
    door_sensor_sim_set_state(true);  // Simulate door closed
    zassert_true(is_door_closed(), "WM should detect door is closed");

}

ZTEST_SUITE(wm_test_suite, NULL, NULL, NULL, NULL, NULL);
