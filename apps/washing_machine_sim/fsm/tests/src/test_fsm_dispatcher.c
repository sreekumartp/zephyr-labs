#include <zephyr/ztest.h>
#include "fsm.h"
#include "l1_system_fsm.h"
#include "l2_wash_cycle_fsm.h"

// Test fixture
static fsm_handle_t fsm;

// This function runs before each test case
static void fsm_dispatcher_before(void *data)
{
    ARG_UNUSED(data);
    // Use the real FSM init function to set the FSM to its starting state.
    fsm_init(&fsm);
}

// --- Test Cases ---

ZTEST(fsm_dispatcher_suite, test_initial_state)
{
    zassert_equal(fsm.system_state, STATE_L1_POWER_OFF, "Initial L1 state should be POWER_OFF");
    zassert_equal(fsm.wash_cycle_state, STATE_L2_IDLE, "Initial L2 state should be IDLE");
}

ZTEST(fsm_dispatcher_suite, test_l1_event_dispatch)
{
    // This event should be handled by the L1 FSM
    fsm_process_event(&fsm, EVENT_POWER_BUTTON_PRESSED);
    zassert_equal(fsm.system_state, STATE_L1_STANDBY, "Dispatcher failed to send event to L1 FSM");
}

ZTEST(fsm_dispatcher_suite, test_l2_event_dispatch_when_running)
{
    // Manually set the state to RUNNING to test L2 dispatch
    fsm.system_state = STATE_L1_RUNNING;
    fsm.wash_cycle_state = STATE_L2_WASHING; // A state that can receive an event

    // This event should be dispatched to the L2 FSM
    fsm_process_event(&fsm, EVENT_TIMER_EXPIRED);

    // Check if the L2 state changed as expected
    zassert_equal(fsm.wash_cycle_state, STATE_L2_DRAINING_WASH, "Dispatcher failed to send event to L2 FSM");
    // Ensure L1 state did not change
    zassert_equal(fsm.system_state, STATE_L1_RUNNING, "L1 state should not change on L2 event");
}

ZTEST(fsm_dispatcher_suite, test_l1_override_event_dispatch_when_running)
{
    // Manually set the state to RUNNING
    fsm.system_state = STATE_L1_RUNNING;
    fsm.wash_cycle_state = STATE_L2_WASHING;

    // This is a high-priority event that L1 should handle even when RUNNING
    fsm_process_event(&fsm, EVENT_PAUSE_BUTTON_PRESSED);

    // Check that the L1 FSM handled the override correctly
    zassert_equal(fsm.system_state, STATE_L1_PAUSED, "Dispatcher failed to prioritize L1 override event");
    // L2 state should remain unchanged
    zassert_equal(fsm.wash_cycle_state, STATE_L2_WASHING, "L2 state should not change on L1 override");
}

ZTEST(fsm_dispatcher_suite, test_l2_completion_event_promotion_to_l1)
{
    // Set up the conditions for the test:
    // L1 is RUNNING, L2 is in its final state before COMPLETE
    fsm.system_state = STATE_L1_RUNNING;
    fsm.wash_cycle_state = STATE_L2_STEAMING; // A state that transitions to COMPLETE
    fsm.program_has_steam = true;

    // This event will cause the L2 FSM to transition to STATE_L2_COMPLETE
    fsm_process_event(&fsm, EVENT_TIMER_EXPIRED);

    // The dispatcher should detect this L2 completion and automatically
    // send the EVENT_CYCLE_FINISHED to the L1 FSM.
    zassert_equal(fsm.wash_cycle_state, STATE_L2_COMPLETE, "L2 cycle should be complete");
    zassert_equal(fsm.system_state, STATE_L1_END, "L1 should transition to END after L2 completes");
}


// --- Test Suite Definition ---
ZTEST_SUITE(fsm_dispatcher_suite, NULL, NULL, fsm_dispatcher_before, NULL, NULL);