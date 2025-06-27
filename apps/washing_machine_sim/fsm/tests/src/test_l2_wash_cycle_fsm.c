#include <zephyr/ztest.h>
#include "fsm.h"
#include "l2_wash_cycle_fsm.h"

// --- Test Fixture ---
// A single FSM handle to be used by all tests in this suite.
static fsm_handle_t fsm;

// --- ZTest Setup ---
// This function runs before each test case, ensuring a clean slate.
static void l2_fsm_before(void *data)
{
    ARG_UNUSED(data);
    // Use the real L2 init function to set the FSM to its starting state.
    l2_fsm_init(&fsm);
    // Also reset program parameters for each test
    fsm.program_has_prewash = false;
    fsm.program_has_heating = false;
    fsm.program_has_steam = false;
}

// --- Test Cases ---

ZTEST(l2_fsm_suite, test_initial_state)
{
    zassert_equal(fsm.wash_cycle_state, STATE_L2_IDLE, "Initial state should be IDLE");
}

ZTEST(l2_fsm_suite, test_start_cycle)
{
    l2_fsm_start(&fsm);
    zassert_equal(fsm.wash_cycle_state, STATE_L2_LOAD_SENSING, "l2_fsm_start should transition to LOAD_SENSING");
}

ZTEST(l2_fsm_suite, test_full_cycle_path_no_options)
{
    // This test verifies the "happy path" when no optional cycles are enabled.

    // Start in the first active state
    fsm.wash_cycle_state = STATE_L2_LOAD_SENSING;
    zassert_equal(fsm.wash_cycle_state, STATE_L2_LOAD_SENSING, "Should start in Load Sensing");

    l2_wash_cycle_process_event(&fsm, EVENT_WEIGHT_CALCULATED);
    zassert_equal(fsm.wash_cycle_state, STATE_L2_DOSING, "Failed transition to Dosing");

    l2_wash_cycle_process_event(&fsm, EVENT_DOSING_COMPLETE);
    zassert_equal(fsm.wash_cycle_state, STATE_L2_PREWASH_CHECK, "Failed transition to Prewash Check");

    // Process the check state (which is synchronous and should immediately transition)
    l2_wash_cycle_process_event(&fsm, 0); // Event doesn't matter for check states
    zassert_equal(fsm.wash_cycle_state, STATE_L2_FILLING, "Should have skipped Prewash");

    l2_wash_cycle_process_event(&fsm, EVENT_WATER_LEVEL_REACHED);
    zassert_equal(fsm.wash_cycle_state, STATE_L2_HEATING_CHECK, "Failed transition to Heating Check");

    l2_wash_cycle_process_event(&fsm, 0);
    zassert_equal(fsm.wash_cycle_state, STATE_L2_WASHING, "Should have skipped Heating");

    l2_wash_cycle_process_event(&fsm, EVENT_TIMER_EXPIRED);
    zassert_equal(fsm.wash_cycle_state, STATE_L2_DRAINING_WASH, "Failed transition to Draining Wash");

    l2_wash_cycle_process_event(&fsm, EVENT_DRUM_EMPTY);
    zassert_equal(fsm.wash_cycle_state, STATE_L2_RINSING, "Failed transition to Rinsing");

    l2_wash_cycle_process_event(&fsm, EVENT_TIMER_EXPIRED);
    zassert_equal(fsm.wash_cycle_state, STATE_L2_DRAINING_RINSE, "Failed transition to Draining Rinse");

    l2_wash_cycle_process_event(&fsm, EVENT_DRUM_EMPTY);
    zassert_equal(fsm.wash_cycle_state, STATE_L2_SPINNING, "Failed transition to Spinning");

    l2_wash_cycle_process_event(&fsm, EVENT_TIMER_EXPIRED);
    zassert_equal(fsm.wash_cycle_state, STATE_L2_STEAM_CHECK, "Failed transition to Steam Check");

    l2_wash_cycle_process_event(&fsm, 0);
    zassert_equal(fsm.wash_cycle_state, STATE_L2_COMPLETE, "Should have skipped Steam");
}

ZTEST(l2_fsm_suite, test_prewash_path_is_taken)
{
    // Set the condition for this path
    fsm.program_has_prewash = true;
    fsm.wash_cycle_state = STATE_L2_PREWASH_CHECK;

    l2_wash_cycle_process_event(&fsm, 0);
    zassert_equal(fsm.wash_cycle_state, STATE_L2_PREWASH, "Did not enter Prewash state when required");
}

ZTEST(l2_fsm_suite, test_heating_path_is_taken)
{
    // Set the condition for this path
    fsm.program_has_heating = true;
    fsm.wash_cycle_state = STATE_L2_HEATING_CHECK;

    l2_wash_cycle_process_event(&fsm, 0);
    zassert_equal(fsm.wash_cycle_state, STATE_L2_HEATING, "Did not enter Heating state when required");
}

ZTEST(l2_fsm_suite, test_steam_path_is_taken)
{
    // Set the condition for this path
    fsm.program_has_steam = true;
    fsm.wash_cycle_state = STATE_L2_STEAM_CHECK;

    l2_wash_cycle_process_event(&fsm, (fsm_event_t)0);
    zassert_equal(fsm.wash_cycle_state, STATE_L2_STEAMING, "Did not enter Steaming state when required");
}


// --- Test Suite Definition ---
ZTEST_SUITE(l2_fsm_suite, NULL, NULL, l2_fsm_before, NULL, NULL);

