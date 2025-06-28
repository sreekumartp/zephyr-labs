#include "event_bus.h"
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>

LOG_MODULE_REGISTER(event_bus, CONFIG_LOG_DEFAULT_LEVEL);

#if defined(CONFIG_EVENT_BUS_USE_CALLBACK)
// --- BEGIN: Corrected Callback Implementation ---

#define MAX_EVENT_HANDLERS 24
#define MAX_EVENTS_PER_HANDLER 64
#define MAX_CONCURRENT_CALLBACKS 24
#define WORK_QUEUE_STACK_SIZE 1024

// --- THE CORRECT METHOD ---
// 1. Define the work queue struct and its stack separately.
static struct k_work_q event_callback_q;
K_THREAD_STACK_DEFINE(event_callback_q_stack, WORK_QUEUE_STACK_SIZE);
// --- END CORRECT METHOD ---

typedef struct {
    event_handler_t handler;
    event_id_t subscribed_events[MAX_EVENTS_PER_HANDLER];
    size_t num_events;
} handler_subscription_t;

static handler_subscription_t handler_subscriptions[MAX_EVENT_HANDLERS];
static int handler_count = 0;

typedef struct {
    struct k_work work;
    event_handler_t handler;
    app_event_t event;
} event_work_item_t;

K_MEM_SLAB_DEFINE(work_item_slab, sizeof(event_work_item_t), MAX_CONCURRENT_CALLBACKS, 4);

static void event_work_handler(struct k_work *work)
{
    event_work_item_t *work_item = CONTAINER_OF(work, event_work_item_t, work);
    if (work_item->handler) {
        work_item->handler(&work_item->event);
    }
    k_mem_slab_free(&work_item_slab, (void *)work_item);
}

int event_bus_register_handler(event_handler_t handler,
                               const event_id_t *events_to_subscribe,
                               size_t num_events)
{
    if (handler_count >= MAX_EVENT_HANDLERS) return -ENOMEM;
    if (num_events > MAX_EVENTS_PER_HANDLER) return -ENOMEM;
    if (!handler || !events_to_subscribe || num_events == 0) return -EINVAL;

    handler_subscriptions[handler_count].handler = handler;
    handler_subscriptions[handler_count].num_events = num_events;
    memcpy(handler_subscriptions[handler_count].subscribed_events,
           events_to_subscribe,
           num_events * sizeof(event_id_t));
    handler_count++;
    return 0;
}
// --- END: Corrected Callback Implementation ---
#endif // CONFIG_EVENT_BUS_USE_CALLBACK

#if defined(CONFIG_EVENT_BUS_USE_POLLING)
// --- BEGIN: Polling-only Implementation ---
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
static void event_dispatcher_thread(void *p1, void *p2, void *p3) {
    ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);
    app_event_t received_event;
    while (1) {
        k_msgq_get(&central_event_q, &received_event, K_FOREVER);
        k_mutex_lock(&subscription_mutex, K_FOREVER);
        for (int i = 0; i < MAX_SUBSCRIPTIONS; i++) {
            if (!subscription_pool[i].is_used) continue;
            for (int j = 0; j < subscription_pool[i].num_events; j++) {
                if (subscription_pool[i].subscribed_events[j] == received_event.id) {
                    if (k_msgq_put(subscription_pool[i].subscriber_msgq, &received_event, K_FOREVER) != 0) {
                         LOG_WRN("Failed to put event %d into sub queue %p", received_event.id, (void*)subscription_pool[i].subscriber_msgq);
                    }
                    break;
                }
            }
        }
        k_mutex_unlock(&subscription_mutex);
    }
}
event_subscription_t* event_bus_subscribe(struct k_msgq *subscriber_msgq, const event_id_t *events_to_subscribe, size_t num_events) {
    if (!subscriber_msgq || !events_to_subscribe || num_events == 0) return NULL;
    if (num_events > MAX_EVENTS_PER_SUBSCRIPTION) return NULL;
    k_mutex_lock(&subscription_mutex, K_FOREVER);
    subscription_t *new_subscription = NULL;
    for (int i = 0; i < MAX_SUBSCRIPTIONS; i++) {
        if (!subscription_pool[i].is_used) {
            new_subscription = &subscription_pool[i];
            new_subscription->is_used = true;
            break;
        }
    }
    if (!new_subscription) {
        k_mutex_unlock(&subscription_mutex);
        return NULL;
    }
    new_subscription->subscriber_msgq = subscriber_msgq;
    new_subscription->num_events = num_events;
    memcpy(new_subscription->subscribed_events, events_to_subscribe, num_events * sizeof(event_id_t));
    k_mutex_unlock(&subscription_mutex);
    return (event_subscription_t*)new_subscription;
}
int event_bus_unsubscribe(event_subscription_t* subscription) {
    if (!subscription) return -EINVAL;
    k_mutex_lock(&subscription_mutex, K_FOREVER);
    ((subscription_t*)subscription)->is_used = false;
    k_mutex_unlock(&subscription_mutex);
    return 0;
}
// --- END: Polling-only Implementation ---
#endif // CONFIG_EVENT_BUS_USE_POLLING


int event_bus_init(void)
{
#if defined(CONFIG_EVENT_BUS_USE_POLLING)
    k_mutex_init(&subscription_mutex);
    for (int i = 0; i < MAX_SUBSCRIPTIONS; i++) {
        subscription_pool[i].is_used = false;
    }
    dispatcher_tid = k_thread_create(&dispatcher_thread_data, dispatcher_stack_area,
                                  K_THREAD_STACK_SIZEOF(dispatcher_stack_area),
                                  event_dispatcher_thread, NULL, NULL, NULL,
                                  DISPATCHER_PRIORITY, 0, K_NO_WAIT);
    if (!dispatcher_tid) return -1;
    k_thread_name_set(dispatcher_tid, "event_dispatcher");
#endif

#if defined(CONFIG_EVENT_BUS_USE_CALLBACK)
    k_work_queue_start(&event_callback_q, event_callback_q_stack,
                       K_THREAD_STACK_SIZEOF(event_callback_q_stack), 5, /* Priority */
                       NULL); /* Options */
    k_thread_name_set(&event_callback_q.thread, "event_callbacks");
#endif

    LOG_INF("Event Bus initialized.");
    return 0;
}

int event_bus_post(const app_event_t *event)
{
    if (!event) return -EINVAL;

#if defined(CONFIG_EVENT_BUS_USE_POLLING)
    return k_msgq_put(&central_event_q, event, K_MSEC(100));

#elif defined(CONFIG_EVENT_BUS_USE_CALLBACK)
    for (int i = 0; i < handler_count; ++i) {
        for (int j = 0; j < handler_subscriptions[i].num_events; ++j) {
            if (handler_subscriptions[i].subscribed_events[j] == event->id) {
                event_work_item_t *work_item;
                if (k_mem_slab_alloc(&work_item_slab, (void **)&work_item, K_NO_WAIT) != 0) {
                    LOG_ERR("Failed to allocate work item.");
                    continue;
                }
                k_work_init(&work_item->work, event_work_handler);
                work_item->handler = handler_subscriptions[i].handler;
                memcpy(&work_item->event, event, sizeof(app_event_t));
                k_work_submit_to_queue(&event_callback_q, &work_item->work);
                break;
            }
        }
    }
    return 0;

#else
    #error "No event bus subscriber mechanism selected"
    return -ENOSYS;
#endif
}