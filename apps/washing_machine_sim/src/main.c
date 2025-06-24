#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "sim_door_sensor.h"
#include "sim_water_level.h"
#include "fsm.h" // Include the new public FSM header
#include "shell_interface.h"


// Create a handle for our hierarchical state machine
static fsm_handle_t fsm;

int main(void) {
    printk("📘 Washing Machine Sim Started\n");
    
    int ret;

	/* Initialize the component under test. */
    ret = door_sensor_sim_init();
    __ASSERT(ret == 0, "door_sensor_sim_init() failed with code %d", ret);
    printk("Door sensor simulation initialization status: %d\n", ret);

    wm_shell_init();

    // Initialize the new FSM to its starting state (POWER_OFF)
    fsm_init(&fsm);
    printk("FSM Initialized. System State: %s\n", fsm_get_system_state_name(fsm.system_state));


    // Main application loop
    while (1) {
        // --- TODO ---
        // This is where the application's main logic will go.
        // It will be responsible for:
        // 1. Reading events from sensors, timers, and the event bus.
        // 2. Calling fsm_process_event(&fsm, received_event);
        // 3. Executing actions based on the new state (e.g., turning on the motor).

        k_sleep(K_MSEC(1000));
    }

    return 0; // Will not be reached
}

// This function is no longer used by the new FSM but kept for reference
bool is_door_closed(void)
{
    return true;
}
