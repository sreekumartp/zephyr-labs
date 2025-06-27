#include <zephyr/ztest.h>
#include "fsm.h"
#include "l1_system_fsm.h"
#include "l2_wash_cycle_fsm.h"

// --- Test Fixture ---
// A single FSM handle to be used by all tests in this suite.
static fsm_handle_t fsm;

// --- Mock Implementations for L2 FSM Dependencies ---
// These are mock functions that satisfy the linker for this unit test.
// They allow us to test the L1 FSM in complete isolation from the L2 FSM.
// We use weak aliases so that if this test is ever linked with the real
// l2_wash_cycle_fsm.c, the real functions will be used instead.

__weak void l2_fsm_init(fsm_handle_t *fsm_handle)
{
    // In this test, we just need to verify that l2_fsm_init resets the state.
    if (fsm_handle) {
        fsm_handle->wash_cycle_state = STATE_L2_IDLE;
    }
}

__weak void l2_fsm_start(fsm_handle_t *fsm_handle)
{
    // The L1 FSM calls this to start the wash cycle. For this test,
    // we just need to mock the transition to the first L2 state.
    if (fsm_handle) {
        fsm_handle->wash_cycle_state = STATE_L2_LOAD_SENSING;
    }
}

// This mock is not strictly required for L1 testing, but included for completeness
// if any test path were to accidentally call it.
__weak void l2_wash_cycle_process_event(fsm_handle_t *fsm, event_id_t event)
{
    // Do nothing in the L1 tests.
}


// --- ZTest Setup ---
// This function runs before each test case, ensuring a clean slate.
static void l1_fsm_before(void *data)
{
    ARG_UNUSED(data);
    // Use the real L1 init function to set the FSM to its starting state.
    l1_fsm_init(&fsm);
}

// --- Test Cases ---

ZTEST(l1_fsm_suite, test_initial_state)
{
    zassert_equal(fsm.system_state, STATE_L1_POWER_OFF, "Initial state should be POWER_OFF");
}

ZTEST(l1_fsm_suite, test_power_cycle)
{
    // Power On
    l1_system_process_event(&fsm, EVENT_POWER_BUTTON_PRESSED);
    zassert_equal(fsm.system_state, STATE_L1_STANDBY, "Failed to transition from POWER_OFF to STANDBY");

    // Power Off from Standby
    l1_system_process_event(&fsm, EVENT_POWER_BUTTON_PRESSED);
    zassert_equal(fsm.system_state, STATE_L1_POWER_OFF, "Failed to transition from STANDBY to POWER_OFF");
}

ZTEST(l1_fsm_suite, test_program_selection_and_start)
{
    // Go to Standby first
    fsm.system_state = STATE_L1_STANDBY;

    l1_system_process_event(&fsm, EVENT_CYCLE_SELECTED);
    zassert_equal(fsm.system_state, STATE_L1_SELECTION, "Failed to transition from STANDBY to SELECTION");

    l1_system_process_event(&fsm, EVENT_START_BUTTON_PRESSED);
    zassert_equal(fsm.system_state, STATE_L1_RUNNING, "Failed to transition from SELECTION to RUNNING");
    // Verify that the L2 FSM was started by our mock function
    zassert_equal(fsm.wash_cycle_state, STATE_L2_LOAD_SENSING, "L2 FSM was not started correctly");
}

ZTEST(l1_fsm_suite, test_running_state_overrides)
{
    // Start in the RUNNING state
    fsm.system_state = STATE_L1_RUNNING;
    bool handled;

    // Test Pause override
    handled = l1_system_process_override_events(&fsm, EVENT_PAUSE_BUTTON_PRESSED);
    zassert_true(handled, "PAUSE event should be handled by override");
    zassert_equal(fsm.system_state, STATE_L1_PAUSED, "Failed to transition from RUNNING to PAUSED");

    // Test Brownout override
    fsm.system_state = STATE_L1_RUNNING; // Reset for next test
    handled = l1_system_process_override_events(&fsm, EVENT_POWER_LOSS_DETECTED);
    zassert_true(handled, "POWER_LOSS event should be handled by override");
    zassert_equal(fsm.system_state, STATE_L1_BROWNOUT, "Failed to transition from RUNNING to BROWNOUT");

    // Test Failure override
    fsm.system_state = STATE_L1_RUNNING; // Reset for next test
    handled = l1_system_process_override_events(&fsm, EVENT_FATAL_FAULT_DETECTED);
    zassert_true(handled, "FATAL_FAULT event should be handled by override");
    zassert_equal(fsm.system_state, STATE_L1_FAILURE, "Failed to transition from RUNNING to FAILURE");
}

ZTEST(l1_fsm_suite, test_running_to_end_transition)
{
    fsm.system_state = STATE_L1_RUNNING;
    // This event is not an override, so it's processed by the main handler
    l1_system_process_event(&fsm, EVENT_CYCLE_FINISHED);
    zassert_equal(fsm.system_state, STATE_L1_END, "Failed to transition from RUNNING to END");
}

ZTEST(l1_fsm_suite, test_paused_state_transitions)
{
    // Start in PAUSED state
    fsm.system_state = STATE_L1_PAUSED;

    // Test Resume
    l1_system_process_event(&fsm, EVENT_START_BUTTON_PRESSED);
    zassert_equal(fsm.system_state, STATE_L1_RUNNING, "Failed to resume from PAUSED to RUNNING");

    // Test Cancel
    fsm.system_state = STATE_L1_PAUSED; // Reset
    fsm.wash_cycle_state = STATE_L2_WASHING; // Pretend L2 was running
    l1_system_process_event(&fsm, EVENT_CANCEL_BUTTON_PRESSED);
    zassert_equal(fsm.system_state, STATE_L1_SELECTION, "Failed to cancel from PAUSED to SELECTION");
    // Verify that the L2 FSM was reset by our mock function
    zassert_equal(fsm.wash_cycle_state, STATE_L2_IDLE, "L2 FSM was not reset on cancel");
}

ZTEST(l1_fsm_suite, test_recovery_state_transitions)
{
    // Test End state recovery
    fsm.system_state = STATE_L1_END;
    l1_system_process_event(&fsm, EVENT_ANY_KEY_PRESSED);
    zassert_equal(fsm.system_state, STATE_L1_SELECTION, "Failed to transition from END to SELECTION");

    // Test Brownout recovery
    fsm.system_state = STATE_L1_BROWNOUT;
    l1_system_process_event(&fsm, EVENT_POWER_RESTORED);
    zassert_equal(fsm.system_state, STATE_L1_RUNNING, "Failed to recover from BROWNOUT to RUNNING");

    // Test Failure recovery
    fsm.system_state = STATE_L1_FAILURE;
    l1_system_process_event(&fsm, EVENT_POWER_BUTTON_PRESSED);
    zassert_equal(fsm.system_state, STATE_L1_STANDBY, "Failed to recover from FAILURE to STANDBY");
}


// --- Test Suite Definition ---
ZTEST_SUITE(l1_fsm_suite, NULL, NULL, l1_fsm_before, NULL, NULL);
