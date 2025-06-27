#include <zephyr/ztest.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "../../include/event_bus.h"

LOG_MODULE_REGISTER(ztest_event_bus, CONFIG_LOG_DEFAULT_LEVEL);

// --- Polling-Mode Tests ---
// All definitions and tests for this suite are now inside this block.
#if defined(CONFIG_EVENT_BUS_USE_POLLING)

static void event_bus_polling_before(void *data)
{
	ARG_UNUSED(data);
	zassert_ok(event_bus_init(), "event_bus_init() failed");
}

K_MSGQ_DEFINE(polling_test_q, sizeof(app_event_t), 4, 4);

static void event_bus_polling_after(void *data)
{
	ARG_UNUSED(data);
	k_msgq_purge(&polling_test_q);
}

ZTEST(event_bus_polling_suite, test_polling_subscribe_receive)
{
	const event_id_t events[] = { EVENT_APP_MESSAGE_SENT };
	event_subscription_t* sub = event_bus_subscribe(&polling_test_q, events, ARRAY_SIZE(events));
	zassert_not_null(sub, "Subscription failed");

	const app_event_t event = { .id = EVENT_APP_MESSAGE_SENT, .payload.s32 = 123 };
	zassert_ok(event_bus_post(&event), "Post failed");

	app_event_t rx_event;
	int ret = k_msgq_get(&polling_test_q, &rx_event, K_MSEC(100));
	zassert_ok(ret, "Did not receive event on subscribed queue");
	zassert_equal(rx_event.id, EVENT_APP_MESSAGE_SENT, "Incorrect event ID received");
	zassert_equal(rx_event.payload.s32, 123, "Incorrect payload received");
}

ZTEST_SUITE(event_bus_polling_suite, NULL, NULL, event_bus_polling_before, event_bus_polling_after, NULL);

#endif // CONFIG_EVENT_BUS_USE_POLLING


// --- Callback-Mode Tests ---
// All definitions and tests for this suite are now inside this block.
#if defined(CONFIG_EVENT_BUS_USE_CALLBACK)

static struct k_sem test_sem;
static app_event_t received_event_storage;

static void test_callback_handler(const app_event_t *event)
{
	memcpy(&received_event_storage, event, sizeof(app_event_t));
	k_sem_give(&test_sem);
}

static void callback_suite_before(void *data)
{
	ARG_UNUSED(data);
	// Initialize the bus first
	zassert_ok(event_bus_init(), "event_bus_init() failed");
	// Then initialize test-specific resources
	k_sem_init(&test_sem, 0, 1);
	memset(&received_event_storage, 0, sizeof(app_event_t));
}

ZTEST(event_bus_callback_suite, test_callback_invoked)
{
	const event_id_t events[] = { EVENT_APP_MESSAGE_SENT };
	zassert_ok(event_bus_register_handler(test_callback_handler, events, ARRAY_SIZE(events)), "Handler registration failed");

	const app_event_t event = { .id = EVENT_APP_MESSAGE_SENT, .payload.s32 = 456 };
	zassert_ok(event_bus_post(&event), "Post failed");

	int ret = k_sem_take(&test_sem, K_MSEC(500));
	zassert_ok(ret, "Callback was not invoked");

	zassert_equal(received_event_storage.id, EVENT_APP_MESSAGE_SENT, "Incorrect event ID in callback");
	zassert_equal(received_event_storage.payload.s32, 456, "Incorrect payload in callback");
}

// THE FIX: The ZTEST_SUITE macro uses the setup function in the correct
// 'test_before' slot (the 4th parameter) which expects the void (*)(void *) signature.
ZTEST_SUITE(event_bus_callback_suite, NULL, NULL, callback_suite_before, NULL, NULL);

#endif // CONFIG_EVENT_BUS_USE_CALLBACK