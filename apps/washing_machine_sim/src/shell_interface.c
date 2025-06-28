#include <zephyr/shell/shell.h>
#include <zephyr/logging/log.h>
#include <stdlib.h>
#include "event_defs.h"
#include "event_bus.h"
#include "shell_interface.h"
#include "fsm.h"
#include "sim_door_sensor.h"

LOG_MODULE_REGISTER(shell_interface, LOG_LEVEL_INF);

static int cmd_start(const struct shell *shell, size_t argc, char **argv) {
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    LOG_INF("Start button pressed, publishing event...");
    // Create and post the event for starting the wash cycle
    // This event can be handled by the FSM or any other component that listens for it
    // Here we use EVENT_START_BUTTON_PRESSED as the event ID
    // and a sample payload of 456 (could be any relevant data)
    // In a real application, you might want to pass more meaningful data in the payload    
    const app_event_t event = { .id = EVENT_START_BUTTON_PRESSED, .payload.s32 = 456 };
    event_bus_post(&event);
    shell_print(shell, "Start button event published.");
    return 0;
}

SHELL_CMD_REGISTER(start, NULL, "Start the wash cycle (event-based)", cmd_start);

// --- Shell Command to Send an Event ---

static int cmd_send_event(const struct shell *shell, size_t argc, char **argv)
{
    if (argc < 2) {
        shell_error(shell, "Please provide an event ID (integer).");
        return -EINVAL;
    }

    event_id_t event_id = (event_id_t)atoi(argv[1]);

    // Simple check to prevent sending wildly invalid event IDs.
    // This could be made more robust if needed.
    if (event_id >= EVENT_ID_COUNT) {
        shell_error(shell, "Invalid event ID: %d", event_id);
        return -EINVAL;
    }

    const app_event_t event_to_post = { .id = event_id };

    event_bus_post(&event_to_post);
    shell_print(shell, "Posted event with ID: %d", event_id);

    return 0;
}
// Register the "send" command with subcommands for each event type
// This makes the shell interface user-friendly and self-documenting.
SHELL_CMD_REGISTER(send_event, NULL, "Send a specific event to the event bus", cmd_send_event);

static int cmd_set_door_state(const struct shell *shell, size_t argc, char **argv)
{
    if (argc < 2) {
        shell_error(shell, "Please provide door state: 0 (open) or 1 (closed).");
        return -EINVAL;
    }

    int state = atoi(argv[1]);
    if (state != 0 && state != 1) {
        shell_error(shell, "Invalid state: %d. Use 0 (open) or 1 (closed).", state);
        return -EINVAL;
    }

    door_sensor_sim_set_state(state ? true : false);
    shell_print(shell, "Door sensor state set to: %s", state ? "closed" : "open");
    return 0;
}

SHELL_CMD_REGISTER(set_door_state, NULL, "Set the simulated door sensor state: 0=open, 1=closed", cmd_set_door_state);


void shell_interface_init(void) {
    // Nothing needed for now
}
