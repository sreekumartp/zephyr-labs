#pragma once

#include "event_defs.h"
#include <zephyr/kernel.h>

/**
 * @brief Opaque handle for an event subscription.
 * The user does not need to know the contents of this struct.
 */
struct event_subscription;
typedef struct event_subscription event_subscription_t;

/**
 * @brief Initializes the Event Bus system.
 *
 * This must be called once at application startup before any events are
 * published or subscribed to. It creates the central event queue and
 * the dispatcher thread that powers the bus.
 *
 * @return 0 on success, or a negative error code on failure.
 */
int event_bus_init(void);

/**
 * @brief De-initializes the Event Bus system.
 *
 * This should be called to gracefully shut down the event bus.
 * It stops the dispatcher thread. This is primarily useful for testing
 * to ensure a clean state between test suites.
 *
 * @return 0 on success, or a negative error code on failure.
 */
int event_bus_deinit(void);

/**
 * @brief Publishes an event to the bus.
 *
 * Any thread can call this function to send an event to the system. The event
 * will be dispatched to all subscribers of that event's ID. This function is
 * thread-safe.
 *
 * @param event A pointer to the event to be published. The contents will be copied.
 * @return 0 on success, or a negative error code on failure (e.g., bus full).
 */
int event_bus_publish(const app_event_t *event);

/**
 * @brief Subscribes a thread's message queue to receive specific events.
 *
 * A component calls this function to register its interest in one or more
 * event types. When a subscribed event is published, it will be sent to the
 * provided message queue.
 *
 * @param subscriber_msgq The message queue of the thread that wants to receive events.
 * This queue must be initialized by the caller.
 * @param events_to_subscribe An array of event IDs to subscribe to.
 * @param num_events The number of events in the array.
 * @return A pointer to a subscription handle on success, or NULL on failure.
 * This handle can be used later to unsubscribe if needed.
 */
event_subscription_t* event_bus_subscribe(struct k_msgq *subscriber_msgq,
                                          const event_id_t *events_to_subscribe,
                                          size_t num_events);
/**
 * @brief Unsubscribes from events.
 *
 * @param subscription The handle returned by event_bus_subscribe().
 * @return 0 on success, or a negative error code on failure.
 */
int event_bus_unsubscribe(event_subscription_t* subscription);