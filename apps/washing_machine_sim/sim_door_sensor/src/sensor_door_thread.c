#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <string.h>

#include "event_bus.h"
#include "event_defs.h"

LOG_MODULE_REGISTER(sensor_door_thread, LOG_LEVEL_INF);

#define SENSOR_STACK_SIZE 1024
#define SENSOR_PRIORITY   5

static struct k_thread sensor_thread_data;
static K_THREAD_STACK_DEFINE(sensor_stack_area, SENSOR_STACK_SIZE);
static struct k_msgq sensor_msgq;
static app_event_t sensor_msgq_buffer[8];

static event_subscription_t *subscription = NULL;

static event_id_t sensor_door_current_state = EVENT_UNKNOWN;

event_id_t sensor_door_get_state(void)
{
    return sensor_door_current_state;
}

static void sensor_door_thread(void *arg1, void *arg2, void *arg3)
{
    ARG_UNUSED(arg1);
    ARG_UNUSED(arg2);
    ARG_UNUSED(arg3);

    app_event_t event;
    // event_id_t previous_state = EVENT_UNKNOWN;
    // event_id_t current_state = EVENT_UNKNOWN;

    LOG_INF("Door sensor thread started");

    while (1) {
        if (k_msgq_get(&sensor_msgq, &event, K_FOREVER) != 0) {
            continue;
        }
        sensor_door_current_state = event.id;
        LOG_INF("Received event: %d", event.id);
    }
}

int sensor_door_start(void)
{
    static const event_id_t sub_ids[] = { EVENT_DOOR_OPENED,
                                          EVENT_DOOR_CLOSED,
                                          EVENT_TEST_DOOR_INPUT };

    k_msgq_init(&sensor_msgq, (char *)sensor_msgq_buffer,
                sizeof(app_event_t), ARRAY_SIZE(sensor_msgq_buffer));

    subscription = event_bus_subscribe(&sensor_msgq, sub_ids, ARRAY_SIZE(sub_ids));
    if (!subscription) {
        LOG_ERR("Failed to subscribe to door events");
        return -1;
    }

    k_thread_create(&sensor_thread_data, sensor_stack_area,
                    K_THREAD_STACK_SIZEOF(sensor_stack_area),
                    sensor_door_thread, NULL, NULL, NULL,
                    SENSOR_PRIORITY, 0, K_NO_WAIT);
    k_thread_name_set(&sensor_thread_data, "sensor_door");

    return 0;
}
