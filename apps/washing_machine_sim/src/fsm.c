
// fsm.c
#include <zephyr/sys/printk.h>
#include "fsm.h"


static fsm_state_t current_state = FSM_STATE_IDLE;

void fsm_step(void) { printk("⏱ FSM step\n"); }

fsm_state_t fsm_get_current_state(void)
{
    return current_state;
}

void fsm_set_state(fsm_state_t new_state)
{
    current_state = new_state;
}
