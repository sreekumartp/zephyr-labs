// fsm.h
#pragma once

typedef enum {
    FSM_STATE_IDLE,
    FSM_STATE_RUNNING,
    FSM_STATE_MAX
} fsm_state_t;


#define FSM_STATE_MAXIMUM (FSM_STATE_MAX - 1) 

fsm_state_t fsm_get_current_state(void);
void fsm_set_state(fsm_state_t new_state);
void fsm_step(void);
