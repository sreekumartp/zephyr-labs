#include <zephyr/ztest.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "event_bus.h"
#include "event_defs.h"
#include "fsm.h"
#include "controller_thread.h"

LOG_MODULE_REGISTER(test_multithreaded, LOG_LEVEL_INF);

ZTEST(multithreaded_tests, test_start_button_event_triggers_fsm)
{
    event_t event = { .type = EVENT_START_BUTTON_PRESSED };
    int ret = event_bus_post(&event);
    zassert_equal(ret, 0, "Failed to publish EVENT_START_BUTTON_PRESSED");

    k_sleep(K_MSEC(100));

    fsm_handle_t *fsm = controller_fsm_get_handle();
    zassert_equal(fsm->system_state, STATE_L1_RUNNING, "Expected L1 state: RUNNING");
    zassert_equal(fsm->wash_cycle_state, STATE_L2_FILLING, "Expected L2 state: FILLING");
}

ZTEST_SUITE(multithreaded_tests, NULL, NULL, NULL, NULL, NULL);
