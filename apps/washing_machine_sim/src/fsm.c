
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
    // Validate state bounds
    if (new_state < 0 || new_state >= FSM_STATE_MAX) {
        printk("⚠️ Invalid FSM state: %d\n", new_state);
        return;
    }    
    current_state = new_state;
    printk("🔄 FSM state changed to: %d\n", new_state);
}
