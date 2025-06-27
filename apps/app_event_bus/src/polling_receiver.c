#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "event_bus.h"

#if defined(CONFIG_EVENT_BUS_USE_POLLING)

LOG_MODULE_REGISTER(polling_receiver, CONFIG_LOG_DEFAULT_LEVEL);

K_MSGQ_DEFINE(polling_q, sizeof(app_event_t), 4, 4);

static void polling_receiver_thread(void)
{
    LOG_INF("Polling receiver thread started.");
    const event_id_t subscribed_events[] = { EVENT_APP_MESSAGE_SENT };
    event_bus_subscribe(&polling_q, subscribed_events, ARRAY_SIZE(subscribed_events));

    app_event_t received_event;
    while (1) {
        k_msgq_get(&polling_q, &received_event, K_FOREVER);
        LOG_WRN("POLLING receiver got event in thread %s. Payload: %d",
                k_thread_name_get(k_current_get()), received_event.payload.s32);
    }
}

K_THREAD_DEFINE(polling_receiver, 1024, polling_receiver_thread, NULL, NULL, NULL, 7, 0, 0);

#endif // CONFIG_EVENT_BUS_USE_POLLING#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "event_bus.h"

#if defined(CONFIG_EVENT_BUS_USE_POLLING)

LOG_MODULE_REGISTER(polling_receiver, CONFIG_LOG_DEFAULT_LEVEL);

K_MSGQ_DEFINE(polling_q, sizeof(app_event_t), 4, 4);

static void polling_receiver_thread(void)
{
    LOG_INF("Polling receiver thread started.");
    const event_id_t subscribed_events[] = { EVENT_DOOR_OPENED };
    event_bus_subscribe(&polling_q, subscribed_events, ARRAY_SIZE(subscribed_events));

    app_event_t received_event;
    while (1) {
        k_msgq_get(&polling_q, &received_event, K_FOREVER);
        LOG_WRN("POLLING receiver got event in thread %s. Payload: %d",
                k_thread_name_get(k_current_get()), received_event.payload.s32);
    }
}

K_THREAD_DEFINE(polling_receiver, 1024, polling_receiver_thread, NULL, NULL, NULL, 7, 0, 0);

#endif // CONFIG_EVENT_BUS_USE_POLLING