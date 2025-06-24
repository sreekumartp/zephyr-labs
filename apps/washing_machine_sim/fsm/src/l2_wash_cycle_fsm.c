#include "l2_wash_cycle_fsm.h"

void l2_fsm_init(fsm_handle_t *fsm)
{
    if (fsm) {
        fsm->wash_cycle_state = STATE_L2_IDLE;
    }
}

void l2_fsm_start(fsm_handle_t *fsm)
{
    if (fsm) {
        fsm->wash_cycle_state = STATE_L2_LOAD_SENSING;
    }
}

void l2_wash_cycle_process_event(fsm_handle_t *fsm, fsm_event_t event)
{
    if (!fsm) {
        return;
    }

    switch (fsm->wash_cycle_state) {
        case STATE_L2_LOAD_SENSING:
            if (event == EVENT_WEIGHT_CALCULATED) {
                fsm->wash_cycle_state = STATE_L2_DOSING;
            }
            break;

        case STATE_L2_DOSING:
            if (event == EVENT_DOSING_COMPLETE) {
                fsm->wash_cycle_state = STATE_L2_PREWASH_CHECK;
            }
            break;

        case STATE_L2_PREWASH_CHECK:
            if (fsm->program_has_prewash) {
                fsm->wash_cycle_state = STATE_L2_PREWASH;
            } else {
                fsm->wash_cycle_state = STATE_L2_FILLING;
            }
            break;

        case STATE_L2_PREWASH:
            if (event == EVENT_TIMER_EXPIRED) {
                fsm->wash_cycle_state = STATE_L2_DRAINING_PRE;
            }
            break;

        case STATE_L2_DRAINING_PRE:
            if (event == EVENT_DRUM_EMPTY) {
                fsm->wash_cycle_state = STATE_L2_FILLING;
            }
            break;

        case STATE_L2_FILLING:
            if (event == EVENT_WATER_LEVEL_REACHED) {
                fsm->wash_cycle_state = STATE_L2_HEATING_CHECK;
            }
            break;

        case STATE_L2_HEATING_CHECK:
            if (fsm->program_has_heating) {
                fsm->wash_cycle_state = STATE_L2_HEATING;
            } else {
                fsm->wash_cycle_state = STATE_L2_WASHING;
            }
            break;

        case STATE_L2_HEATING:
            if (event == EVENT_TEMP_REACHED) {
                fsm->wash_cycle_state = STATE_L2_WASHING;
            }
            break;

        case STATE_L2_WASHING:
            if (event == EVENT_TIMER_EXPIRED) {
                fsm->wash_cycle_state = STATE_L2_DRAINING_WASH;
            }
            break;

        case STATE_L2_DRAINING_WASH:
            if (event == EVENT_DRUM_EMPTY) {
                fsm->wash_cycle_state = STATE_L2_RINSING;
            }
            break;

        case STATE_L2_RINSING:
            if (event == EVENT_TIMER_EXPIRED) {
                fsm->wash_cycle_state = STATE_L2_DRAINING_RINSE;
            }
            break;

        case STATE_L2_DRAINING_RINSE:
            if (event == EVENT_DRUM_EMPTY) {
                fsm->wash_cycle_state = STATE_L2_SPINNING;
            }
            break;

        case STATE_L2_SPINNING:
            if (event == EVENT_TIMER_EXPIRED) {
                fsm->wash_cycle_state = STATE_L2_STEAM_CHECK;
            }
            break;

        case STATE_L2_STEAM_CHECK:
            if (fsm->program_has_steam) {
                fsm->wash_cycle_state = STATE_L2_STEAMING;
            } else {
                fsm->wash_cycle_state = STATE_L2_COMPLETE;
            }
            break;

        case STATE_L2_STEAMING:
            if (event == EVENT_TIMER_EXPIRED) {
                fsm->wash_cycle_state = STATE_L2_COMPLETE;
            }
            break;

        default:
            // No action needed for IDLE or COMPLETE states here.
            break;
    }
}
