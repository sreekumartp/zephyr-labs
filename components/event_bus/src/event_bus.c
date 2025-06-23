#include "event_bus.h"
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(event_bus, CONFIG_LOG_DEFAULT_LEVEL);

#define MAX_SUBSCRIPTIONS 16
#define MAX_EVENTS_PER_SUBSCRIPTION 24
#define CENTRAL_QUEUE_CAPACITY 32

typedef struct {
    bool is_used;
    struct k_msgq *subscriber_msgq;
    event_id_t subscribed_events[MAX_EVENTS_PER_SUBSCRIPTION];
    size_t num_events;
} subscription_t;

K_MSGQ_DEFINE(central_event_q, sizeof(app_event_t), CENTRAL_QUEUE_CAPACITY, 4);
static subscription_t subscription_pool[MAX_SUBSCRIPTIONS];
static K_MUTEX_DEFINE(subscription_mutex);

#define DISPATCHER_STACK_SIZE 1024
#define DISPATCHER_PRIORITY 5

K_THREAD_STACK_DEFINE(dispatcher_stack_area, DISPATCHER_STACK_SIZE);
static struct k_thread dispatcher_thread_data;
static k_tid_t dispatcher_tid = NULL;

// Semaphore to signal the dispatcher thread to exit
static K_SEM_DEFINE(dispatcher_exit_sem, 0, 1);

void event_dispatcher_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);
    app_event_t received_event;

    while (k_sem_take(&dispatcher_exit_sem, K_NO_WAIT) != 0) {
        // Use a timeout on k_msgq_get so the thread can periodically check the exit semaphore
        if (k_msgq_get(&central_event_q, &received_event, K_MSEC(500)) != 0) {
            continue;
        }

        if (k_mutex_lock(&subscription_mutex, K_MSEC(500)) != 0) {
            LOG_ERR("Failed to lock subscription mutex.");
            continue;
        }

        for (int i = 0; i < MAX_SUBSCRIPTIONS; i++) {
            if (!subscription_pool[i].is_used) continue;
            for (int j = 0; j < subscription_pool[i].num_events; j++) {
                if (subscription_pool[i].subscribed_events[j] == received_event.id) {
                    // Block forever if a subscriber's queue is full. This prevents message loss.
                    if (k_msgq_put(subscription_pool[i].subscriber_msgq, &received_event, K_FOREVER) != 0) {
                         LOG_WRN("Failed to put event %d into sub queue %p", received_event.id, (void*)subscription_pool[i].subscriber_msgq);
                    }
                    break;
                }
            }
        }
        k_mutex_unlock(&subscription_mutex);
    }
     LOG_INF("Dispatcher thread exiting.");
}

int event_bus_init(void)
{
    k_mutex_init(&subscription_mutex);
    k_sem_reset(&dispatcher_exit_sem);
    for (int i = 0; i < MAX_SUBSCRIPTIONS; i++) {
        subscription_pool[i].is_used = false;
    }
    k_msgq_purge(&central_event_q);
    dispatcher_tid = k_thread_create(&dispatcher_thread_data, dispatcher_stack_area,
                                  K_THREAD_STACK_SIZEOF(dispatcher_stack_area),
                                  event_dispatcher_thread, NULL, NULL, NULL,
                                  DISPATCHER_PRIORITY, 0, K_NO_WAIT);
    if (!dispatcher_tid) {
        LOG_ERR("Failed to create event dispatcher thread.");
        return -1;
    }
    k_thread_name_set(dispatcher_tid, "event_dispatcher");
    LOG_INF("Event Bus initialized successfully.");
    return 0;
}

int event_bus_deinit(void)
{
    if (dispatcher_tid) {
        k_sem_give(&dispatcher_exit_sem);
        k_thread_join(&dispatcher_thread_data, K_MSEC(1000));
        dispatcher_tid = NULL;
        LOG_INF("Event bus de-initialized.");
    }
    return 0;
}

int event_bus_publish(const app_event_t *event)
{
    if (!event || event->id >= EVENT_ID_COUNT) {
        return -EINVAL;
    }

    app_event_t event_copy = *event;
    event_copy.sender_tid = k_current_get();

    if (k_msgq_put(&central_event_q, &event_copy, K_MSEC(100)) != 0) {
        LOG_ERR("Failed to publish event ID %d. Central queue may be full.", event->id);
        return -ENOMEM;
    }
    return 0;
}

event_subscription_t* event_bus_subscribe(struct k_msgq *subscriber_msgq,
                                          const event_id_t *events_to_subscribe,
                                          size_t num_events)
{
    if (!subscriber_msgq || !events_to_subscribe || num_events == 0) {
        LOG_ERR("Invalid arguments for subscription.");
        return NULL;
    }
    if (num_events > MAX_EVENTS_PER_SUBSCRIPTION) {
        LOG_ERR("Cannot subscribe to %zu events. Max is %d.", num_events, MAX_EVENTS_PER_SUBSCRIPTION);
        return NULL;
    }
    if (k_mutex_lock(&subscription_mutex, K_MSEC(500)) != 0) {
        LOG_ERR("Failed to lock subscription mutex for subscribing.");
        return NULL;
    }
    subscription_t *new_subscription = NULL;
    for (int i = 0; i < MAX_SUBSCRIPTIONS; i++) {
        if (!subscription_pool[i].is_used) {
            new_subscription = &subscription_pool[i];
            new_subscription->is_used = true;
            break;
        }
    }
    if (!new_subscription) {
        LOG_ERR("Failed to find an empty subscription slot. MAX_SUBSCRIPTIONS may be too low.");
        k_mutex_unlock(&subscription_mutex);
        return NULL;
    }
    new_subscription->subscriber_msgq = subscriber_msgq;
    new_subscription->num_events = num_events;
    memcpy(new_subscription->subscribed_events, events_to_subscribe, num_events * sizeof(event_id_t));
    k_mutex_unlock(&subscription_mutex);
    LOG_INF("New subscription successful for message queue at %p.", (void*)new_subscription->subscriber_msgq);
    return (event_subscription_t*)new_subscription;
}

int event_bus_unsubscribe(event_subscription_t* subscription)
{
    if (!subscription) {
        return -EINVAL;
    }

    if (k_mutex_lock(&subscription_mutex, K_MSEC(500)) != 0) {
        LOG_ERR("Failed to lock subscription mutex for unsubscribing.");
        return -EAGAIN;
    }

    subscription_t *sub_to_remove = (subscription_t*)subscription;

    if (sub_to_remove < &subscription_pool[0] || sub_to_remove > &subscription_pool[MAX_SUBSCRIPTIONS - 1]) {
        k_mutex_unlock(&subscription_mutex);
        return -EINVAL;
    }

    if (sub_to_remove->is_used) {
        sub_to_remove->is_used = false;
        LOG_INF("Unsubscribed successfully for message queue at %p.", (void*)sub_to_remove->subscriber_msgq);
    }

    k_mutex_unlock(&subscription_mutex);
    return 0;
}