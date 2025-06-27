#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "event_bus.h"

#if defined(CONFIG_EVENT_BUS_USE_CALLBACK)

LOG_MODULE_REGISTER(callback_receiver, CONFIG_LOG_DEFAULT_LEVEL);

// This is an external reference to the processing thread's message queue.
extern struct k_msgq processing_msgq;

// The callback is now lightweight and non-blocking.
static void app_event_handler(const app_event_t *event)
{
    // Don't do any work here. Just hand the event off.
    int ret = k_msgq_put(&processing_msgq, event, K_NO_WAIT);
    if (ret != 0) {
        LOG_WRN("Could not enqueue event for processing thread, queue may be full.");
    } else {
        LOG_INF("Callback receiver ENQUEUED event %d for processing thread.", event->id);
    }
}

static int register_callback(void)
{
    const event_id_t subscribed_events[] = { EVENT_DOOR_OPENED };
    LOG_INF("Registering lightweight hand-off callback for EVENT_APP_MESSAGE_SENT.");
    return event_bus_register_handler(app_event_handler,
                                      subscribed_events,
                                      ARRAY_SIZE(subscribed_events));
}

SYS_INIT(register_callback, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

#endif // CONFIG_EVENT_BUS_USE_CALLBACK