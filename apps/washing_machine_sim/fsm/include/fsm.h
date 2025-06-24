#pragma once

#include <stdint.h>
#include <stdbool.h>

// --- Level 1: System Mode States ---
typedef enum {
    STATE_L1_POWER_OFF,
    STATE_L1_STANDBY,
    STATE_L1_SELECTION,
    STATE_L1_RUNNING,
    STATE_L1_PAUSED,
    STATE_L1_END,
    STATE_L1_BROWNOUT,
    STATE_L1_FAILURE,
} system_state_t;

// --- Level 2: Wash Cycle States ---
typedef enum {
    STATE_L2_IDLE,
    STATE_L2_LOAD_SENSING,
    STATE_L2_DOSING,
    STATE_L2_PREWASH_CHECK,
    STATE_L2_PREWASH,
    STATE_L2_DRAINING_PRE,
    STATE_L2_FILLING,
    STATE_L2_HEATING_CHECK,
    STATE_L2_HEATING,
    STATE_L2_WASHING,
    STATE_L2_DRAINING_WASH,
    STATE_L2_RINSING,
    STATE_L2_DRAINING_RINSE,
    STATE_L2_SPINNING,
    STATE_L2_STEAM_CHECK,
    STATE_L2_STEAMING,
    STATE_L2_COMPLETE,
} wash_cycle_state_t;

// --- System-Wide Events (Triggers) ---
typedef enum {
    EVENT_POWER_BUTTON_PRESSED,
    EVENT_CYCLE_SELECTED,
    EVENT_START_BUTTON_PRESSED,
    EVENT_PAUSE_BUTTON_PRESSED,
    EVENT_CANCEL_BUTTON_PRESSED,
    EVENT_ANY_KEY_PRESSED,
    EVENT_WEIGHT_CALCULATED,
    EVENT_DOSING_COMPLETE,
    EVENT_TIMER_EXPIRED,
    EVENT_DRUM_EMPTY,
    EVENT_WATER_LEVEL_REACHED,
    EVENT_TEMP_REACHED,
    EVENT_POWER_LOSS_DETECTED,
    EVENT_POWER_RESTORED,
    EVENT_FATAL_FAULT_DETECTED,
    EVENT_CYCLE_FINISHED,
} fsm_event_t;

// --- FSM Handle ---
typedef struct {
    system_state_t system_state;
    wash_cycle_state_t wash_cycle_state;
    // Program-specific parameters
    bool program_has_prewash;
    bool program_has_heating;
    bool program_has_steam;
} fsm_handle_t;

// --- Public API Functions ---
void fsm_init(fsm_handle_t *fsm);
void fsm_process_event(fsm_handle_t *fsm, fsm_event_t event);
const char* fsm_get_system_state_name(system_state_t state);
const char* fsm_get_wash_cycle_state_name(wash_cycle_state_t state);

