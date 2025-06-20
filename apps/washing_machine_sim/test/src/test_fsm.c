#include <zephyr/ztest.h>
#include "fsm.h"

ZTEST(fsm_suite, test_default_state_is_idle)
{
    zassert_equal(fsm_get_current_state(), FSM_STATE_IDLE, "Should start in IDLE state");
}

ZTEST(fsm_suite, test_state_transition)
{
    fsm_set_state(FSM_STATE_RUNNING);
    zassert_equal(fsm_get_current_state(), FSM_STATE_RUNNING, "Should transition to RUNNING");
}
ZTEST(fsm_suite, test_state_maximum)
{
    zassert_equal(FSM_STATE_MAXIMUM, FSM_STATE_MAX - 1, "FSM_STATE_MAXIMUM should be one less than FSM_STATE_MAX");
}


ZTEST_SUITE(fsm_suite, NULL, NULL, NULL, NULL, NULL);
