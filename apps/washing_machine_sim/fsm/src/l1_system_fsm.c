#include "l1_system_fsm.h"
#include "l2_wash_cycle_fsm.h" // L1 needs to start/reset L2

void l1_fsm_init(fsm_handle_t *fsm)
{
    if (fsm) {
        fsm->system_state = STATE_L1_POWER_OFF;
    }
}

bool l1_system_process_override_events(fsm_handle_t *fsm, fsm_event_t event)
{
    if (!fsm || fsm->system_state != STATE_L1_RUNNING) {
        return false;
    }

    switch (event) {
        case EVENT_PAUSE_BUTTON_PRESSED:
            fsm->system_state = STATE_L1_PAUSED;
            return true;
        case EVENT_POWER_LOSS_DETECTED:
            fsm->system_state = STATE_L1_BROWNOUT;
            // In a real system, you would save state to NVM here
            return true;
        case EVENT_FATAL_FAULT_DETECTED:
            fsm->system_state = STATE_L1_FAILURE;
            return true;
        default:
            return false;
    }
}

void l1_system_process_event(fsm_handle_t *fsm, fsm_event_t event)
{
    if (!fsm) {
        return;
    }

    switch (fsm->system_state) {
        case STATE_L1_POWER_OFF:
            if (event == EVENT_POWER_BUTTON_PRESSED) {
                fsm->system_state = STATE_L1_STANDBY;
            }
            break;

        case STATE_L1_STANDBY:
            if (event == EVENT_CYCLE_SELECTED) {
                fsm->system_state = STATE_L1_SELECTION;
            } else if (event == EVENT_POWER_BUTTON_PRESSED) {
                fsm->system_state = STATE_L1_POWER_OFF;
            }
            break;

        case STATE_L1_SELECTION:
            if (event == EVENT_START_BUTTON_PRESSED) {
                fsm->system_state = STATE_L1_RUNNING;
                // Activate the nested FSM
                l2_fsm_start(fsm);
            } else if (event == EVENT_POWER_BUTTON_PRESSED) {
                fsm->system_state = STATE_L1_POWER_OFF;
            }
            break;

        case STATE_L1_RUNNING:
            // This state is only processed here for non-override events.
            // The only such event is the completion signal from L2.
            if (event == EVENT_CYCLE_FINISHED) {
                fsm->system_state = STATE_L1_END;
            }
            break;

        case STATE_L1_PAUSED:
            if (event == EVENT_START_BUTTON_PRESSED) {
                fsm->system_state = STATE_L1_RUNNING; // Resume
            } else if (event == EVENT_CANCEL_BUTTON_PRESSED) {
                fsm->system_state = STATE_L1_SELECTION;
                l2_fsm_init(fsm); // Reset the L2 FSM to Idle
            }
            break;

        case STATE_L1_END:
            if (event == EVENT_ANY_KEY_PRESSED) {
                fsm->system_state = STATE_L1_SELECTION;
            }
            break;

        case STATE_L1_BROWNOUT:
            if (event == EVENT_POWER_RESTORED) {
                fsm->system_state = STATE_L1_RUNNING;
            }
            break;

        case STATE_L1_FAILURE:
            if (event == EVENT_POWER_BUTTON_PRESSED) {
                fsm->system_state = STATE_L1_STANDBY;
            }
            break;
        
        default:
            // No action needed for other states
            break;
    }
}
