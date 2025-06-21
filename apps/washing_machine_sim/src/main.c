#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "sim_door_sensor.h"
#include "sim_water_level.h"
#include "fsm.h"
#include "shell_interface.h"


static int fsm_state = FSM_STATE_IDLE;
static int counter = 0;

int main(void) {
    printk("📘 Washing Machine Sim Started\n");
    
    int ret;

	/* Initialize the component under test. */
    ret = door_sensor_sim_init();
    __ASSERT(ret == 0, "door_sensor_sim_init() failed with code %d", ret);
    printk("Door sensor simulation initialization status: %d\n", ret);


    // door_sensor_sim_set_state(false);
    // water_level_sim_set_state(false);
    
    wm_shell_init();



    while (1) {


        fsm_set_state(fsm_state); // Update FSM state

        counter++;
        if (counter >= 4) { // 4 * 500ms = 2000ms = 2 seconds
            fsm_state = (fsm_state + 1) % FSM_STATE_MAX; // Cycle through states
            // Ensure state stays within valid range
            if (fsm_state < 0 || fsm_state >= FSM_STATE_MAX) {
                fsm_state = FSM_STATE_IDLE;
            }            
            counter = 0;
        }
        k_sleep(K_MSEC(500));
    }

}

bool is_door_closed(void)
{
    return true;
}