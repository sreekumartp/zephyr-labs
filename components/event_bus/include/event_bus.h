#pragma once

#include "event_defs.h"
#include <zephyr/kernel.h>

#if defined(CONFIG_EVENT_BUS_USE_POLLING)
/**
 * @brief Opaque handle for an event subscription.
 */
struct event_subscription;
typedef struct event_subscription event_subscription_t;

/**
 * @brief Subscribes a thread's message queue to receive specific events.
 */
event_subscription_t* event_bus_subscribe(struct k_msgq *subscriber_msgq,
                                          const event_id_t *events_to_subscribe,
                                          size_t num_events);
/**
 * @brief Unsubscribes from events.
 */
int event_bus_unsubscribe(event_subscription_t* subscription);
#endif // CONFIG_EVENT_BUS_USE_POLLING

#if defined(CONFIG_EVENT_BUS_USE_CALLBACK)
/**
 * @brief Defines the function signature for a callback-based event handler.
 */
typedef void (*event_handler_t)(const app_event_t *event);

/**
 * @brief Registers a callback function to be invoked only for specific, subscribed events.
 *
 * @param handler The callback function to execute.
 * @param events_to_subscribe An array of event IDs this handler should be triggered for.
 * @param num_events The number of events in the array.
 * @return 0 on success, or a negative error code on failure.
 */
int event_bus_register_handler(event_handler_t handler,
                               const event_id_t *events_to_subscribe,
                               size_t num_events);
#endif // CONFIG_EVENT_BUS_USE_CALLBACK

/**
 * @brief Initializes the Event Bus system.
 */
int event_bus_init(void);

/**
 * @brief Posts an event to all subscribers using the configured mechanism.
 */
int event_bus_post(const app_event_t *event);