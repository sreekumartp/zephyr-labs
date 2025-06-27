#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "event_bus.h"

LOG_MODULE_REGISTER(sender, CONFIG_LOG_DEFAULT_LEVEL);

static void sender_thread(void)
{
    int counter = 0;
    while (1) {
        k_msleep(3000); // Wait 3 seconds
        const app_event_t msg_event = {
            .id = EVENT_DOOR_OPENED,
            .payload = { .s32 = counter++ }
        };
        LOG_INF("Posting event from thread %s...", k_thread_name_get(k_current_get()));
        event_bus_post(&msg_event);
    }
}

K_THREAD_DEFINE(sender, 1024, sender_thread, NULL, NULL, NULL, 7, 0, 0);