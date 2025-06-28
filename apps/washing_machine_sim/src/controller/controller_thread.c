#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "event_bus.h"
#include "event_defs.h"
#include "fsm.h"
#include "controller_thread.h"

LOG_MODULE_REGISTER(controller_thread, LOG_LEVEL_INF);

// --- Message Queue Definition ---
// Defines the message queue for the FSM, matching the extern in the header.
K_MSGQ_DEFINE(fsm_msgq, sizeof(app_event_t), 16, 4);

// --- Thread Definition ---
#define CONTROLLER_STACK_SIZE 1024
#define CONTROLLER_PRIORITY 5
K_THREAD_STACK_DEFINE(controller_stack, CONTROLLER_STACK_SIZE);
static struct k_thread controller_thread_data;

// --- FSM Handle ---
static fsm_handle_t fsm;

fsm_handle_t *controller_fsm_get_handle(void) {
    return &fsm;
}

// --- Lightweight Event Callback ---
/**
 * @brief A lightweight, non-blocking callback to receive events for the FSM.
 *
 * This function's only job is to place the received event into the FSM's
 * message queue. All processing is deferred to the controller thread.
 */
static void fsm_event_callback(const app_event_t *event)
{
    LOG_INF("FSM Controller thread callback received event ID: %d", event->id);
    int ret = k_msgq_put(&fsm_msgq, event, K_NO_WAIT);
    if (ret != 0) {
        LOG_WRN("Failed to enqueue event for FSM thread, queue may be full.");
    }
}

// --- Controller Thread Entry Point ---
/**
 * @brief The main entry point for the controller thread.
 *
 * This thread waits indefinitely for events to arrive on its message queue,
 * then processes them through the main FSM dispatcher.
 */
static void controller_thread_entry(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    app_event_t received_event;
    fsm_init(&fsm);
    LOG_INF("FSM Controller thread started, waiting for events.");

    while (1) {
        // Wait forever for an event to arrive from the callback
        k_msgq_get(&fsm_msgq, &received_event, K_FOREVER);

        LOG_INF("Controller thread processing event ID: %d", received_event.id);
        fsm_process_event(&fsm, received_event.id);
    }
}

// --- Initialization Function ---
int controller_thread_init(void)
{
    // List of all events the FSM cares about.
    static const event_id_t subscribed_events[] = {
        EVENT_ANY_KEY_PRESSED,
        EVENT_CANCEL_BUTTON_PRESSED,
        EVENT_CYCLE_FINISHED,
        EVENT_CYCLE_SELECTED,
        EVENT_DOOR_CLOSED,
        EVENT_DOOR_OPENED,        
        EVENT_DOSING_COMPLETE,
        EVENT_DRUM_EMPTY,
        EVENT_FATAL_FAULT_DETECTED,
        EVENT_PAUSE_BUTTON_PRESSED,
        EVENT_POWER_BUTTON_PRESSED,
        EVENT_POWER_LOSS_DETECTED,
        EVENT_POWER_RESTORED,
        EVENT_START_BUTTON_PRESSED,
        EVENT_TEMP_REACHED,
        EVENT_TIMER_EXPIRED,
        EVENT_WATER_LEVEL_REACHED,
        EVENT_WEIGHT_CALCULATED,
    };

    // Register our lightweight callback with the event bus
    int ret = event_bus_register_handler(fsm_event_callback,
                                         subscribed_events,
                                         ARRAY_SIZE(subscribed_events));
    if (ret != 0) {
        LOG_ERR("Failed to register FSM event handler!");
        return -1;
    }
    else {
        LOG_INF("FSM event handler registered successfully.");
    }

    // Create and start the controller thread
    k_thread_create(&controller_thread_data, controller_stack,
                    K_THREAD_STACK_SIZEOF(controller_stack),
                    controller_thread_entry, NULL, NULL, NULL,
                    CONTROLLER_PRIORITY, 0, K_NO_WAIT);
    k_thread_name_set(&controller_thread_data, "fsm_controller");

    return 0;
}