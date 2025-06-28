#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "event_bus.h"
#include "controller_thread.h"
#include "shell_interface.h"
#include "door_sensor.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void) {
    LOG_INF("System Init: Main");

    // Initialize the event bus first, as others depend on it.
    if (event_bus_init() != 0) {
        LOG_ERR("Failed to initialize event bus!");
        return 1;
    }

    // Initialize our new FSM controller thread.
    // This will register the callback and start the thread.
    if (controller_thread_init() != 0) {
        LOG_ERR("Failed to initialize controller thread!");
        return 1;
    }

    // Initialize the shell for user input.
    shell_interface_init();
    door_sensor_init();
    LOG_INF("All components initialized. System is now running.");

    // The main thread can now sleep, as all work is done in other threads.
    while (1) {
        k_sleep(K_FOREVER);
    }
    return 0; // Should never reach here
}