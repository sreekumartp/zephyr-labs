#include "fsm.h"
#include "l1_system_fsm.h"
#include "l2_wash_cycle_fsm.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(fsm_main, CONFIG_LOG_DEFAULT_LEVEL);


void fsm_init(fsm_handle_t *fsm)
{
    if (fsm) {
        l1_fsm_init(fsm);
        l2_fsm_init(fsm);
        fsm->program_has_prewash = false;
        fsm->program_has_heating = false;
        fsm->program_has_steam = false;
    }
}

void fsm_process_event(fsm_handle_t *fsm, fsm_event_t event)
{
    if (!fsm) {
        return;
    }

    system_state_t original_l1_state = fsm->system_state;
    wash_cycle_state_t original_l2_state = fsm->wash_cycle_state;

    // L1 FSM handles high-priority override events first.
    // These are events that can interrupt the RUNNING state.
    bool event_handled_by_l1 = l1_system_process_override_events(fsm, event);

    if (!event_handled_by_l1) {
        // If not an override, dispatch based on the current L1 state.
        if (fsm->system_state == STATE_L1_RUNNING) {
            l2_wash_cycle_process_event(fsm, event);
        } else {
            l1_system_process_event(fsm, event);
        }
    }

    // A special case: if the L2 machine has just completed,
    // we need to inject an event into the L1 machine to signal this.
    if (original_l2_state != STATE_L2_COMPLETE && fsm->wash_cycle_state == STATE_L2_COMPLETE) {
        l1_system_process_event(fsm, EVENT_CYCLE_FINISHED);
    }
    
    if (original_l1_state != fsm->system_state) {
        LOG_INF("L1 State Change: %s -> %s", 
                fsm_get_system_state_name(original_l1_state), 
                fsm_get_system_state_name(fsm->system_state));
    }

    if (original_l2_state != fsm->wash_cycle_state) {
        LOG_INF("L2 State Change: %s -> %s", 
                fsm_get_wash_cycle_state_name(original_l2_state), 
                fsm_get_wash_cycle_state_name(fsm->wash_cycle_state));
    }
}


const char* fsm_get_system_state_name(system_state_t state)
{
    switch(state) {
        case STATE_L1_POWER_OFF: return "Power Off";
        case STATE_L1_STANDBY: return "Standby";
        case STATE_L1_SELECTION: return "Selection";
        case STATE_L1_RUNNING: return "Running";
        case STATE_L1_PAUSED: return "Paused";
        case STATE_L1_END: return "End";
        case STATE_L1_BROWNOUT: return "Brownout";
        case STATE_L1_FAILURE: return "Failure";
    }
    return "Unknown L1";
}

const char* fsm_get_wash_cycle_state_name(wash_cycle_state_t state)
{
    switch(state) {
        case STATE_L2_IDLE: return "Idle";
        case STATE_L2_LOAD_SENSING: return "Load Sensing";
        case STATE_L2_DOSING: return "Dosing";
        case STATE_L2_PREWASH_CHECK: return "Pre-Wash Check";
        case STATE_L2_PREWASH: return "Pre-Wash";
        case STATE_L2_DRAINING_PRE: return "Draining (Pre)";
        case STATE_L2_FILLING: return "Filling";
        case STATE_L2_HEATING_CHECK: return "Heating Check";
        case STATE_L2_HEATING: return "Heating";
        case STATE_L2_WASHING: return "Washing";
        case STATE_L2_DRAINING_WASH: return "Draining (Wash)";
        case STATE_L2_RINSING: return "Rinsing";
        case STATE_L2_DRAINING_RINSE: return "Draining (Rinse)";
        case STATE_L2_SPINNING: return "Spinning";
        case STATE_L2_STEAM_CHECK: return "Steam Check";
        case STATE_L2_STEAMING: return "Steaming";
        case STATE_L2_COMPLETE: return "Complete";
    }
    return "Unknown L2";
}
