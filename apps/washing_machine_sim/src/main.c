#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "sim_door_sensor.h"
#include "sim_water_level.h"
#include "fsm.h"
#include "shell_interface.h"




int main(void) {
    printk("📘 Washing Machine Sim Started\n");
    door_sensor_sim_set_state(false);
    water_level_sim_set_state(false);
    wm_shell_init();

    while (1) {

        static int state = FSM_STATE_IDLE;
        fsm_set_state(state); // Update FSM state

        static int counter = 0;
        counter++;
        if (counter >= 4) { // 4 * 500ms = 2000ms = 2 seconds
            state = (state + 1) % FSM_STATE_MAX; // Cycle through states
            counter = 0;
        }
        k_sleep(K_MSEC(500));
    }

}

bool is_door_closed(void)
{
    return true;
}