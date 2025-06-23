#include <zephyr/kernel.h>
#include <zephyr/ztest.h>
#include "event_bus.h"

// --- Test Configuration & Fixtures ---

K_MSGQ_DEFINE(single_sub_msgq, sizeof(app_event_t), 8, 4);
K_MSGQ_DEFINE(multi_sub1_msgq, sizeof(app_event_t), 8, 4);
K_MSGQ_DEFINE(multi_sub2_msgq, sizeof(app_event_t), 8, 4);
K_MSGQ_DEFINE(unsub_msgq, sizeof(app_event_t), 8, 4);
K_MSGQ_DEFINE(payload_msgq, sizeof(app_event_t), 8, 4);
K_MSGQ_DEFINE(multi_pub_sub1_msgq, sizeof(app_event_t), 8, 4);
K_MSGQ_DEFINE(multi_pub_sub2_msgq, sizeof(app_event_t), 8, 4);
K_MSGQ_DEFINE(dummy_msgq_for_limit_test, sizeof(app_event_t), 1, 4);
K_MSGQ_DEFINE(stress_sub_msgq, sizeof(app_event_t), 64, 4);
K_MSGQ_DEFINE(unsub_test_msgq, sizeof(app_event_t), 8, 4);


K_SEM_DEFINE(sub1_ready_sem, 0, 1);
K_SEM_DEFINE(sub2_ready_sem, 0, 1);
K_SEM_DEFINE(test_done_sem, 0, 4);
K_SEM_DEFINE(priority_test_sem, 0, 1);

#define STACK_SIZE 1024
K_THREAD_STACK_DEFINE(sub1_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(sub2_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(pub1_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(pub2_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(stress_pub_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(stress_sub_stack, STACK_SIZE);


static struct k_thread sub1_thread_data;
static struct k_thread sub2_thread_data;
static struct k_thread pub1_thread_data;
static struct k_thread pub2_thread_data;
static struct k_thread stress_pub_thread_data;
static struct k_thread stress_sub_thread_data;


static bool sub1_thread_started, sub2_thread_started, pub1_thread_started, pub2_thread_started, stress_pub_thread_started, stress_sub_thread_started;

#define MAX_SUBSCRIPTIONS 16
#define STRESS_TEST_EVENT_COUNT 1000

// --- Test Helper Threads ---

void generic_subscriber_thread_entry(void *p1, void *p2, void *p3)
{
    struct k_msgq *my_msgq = (struct k_msgq *)p1;
    const event_id_t *events_to_sub = (const event_id_t *)p2;
    struct k_sem *my_ready_sem = (my_msgq == &multi_sub1_msgq) ? &sub1_ready_sem : &sub2_ready_sem;
    event_subscription_t* sub_handle = event_bus_subscribe(my_msgq, events_to_sub, (size_t)p3);
    zassert_not_null(sub_handle, "Subscription failed");
    k_sem_give(my_ready_sem);
    app_event_t received_event;
    int ret = k_msgq_get(my_msgq, &received_event, K_MSEC(1000));
    zassert_equal(ret, 0, "Subscriber failed to receive event");
    k_sem_give(&test_done_sem);
}

void selective_subscriber_thread_entry(void *p1, void *p2, void *p3)
{
    struct k_msgq *my_msgq = (struct k_msgq *)p1;
    event_id_t expected_event_id = (event_id_t)(intptr_t)p2;
    struct k_sem *my_ready_sem = (struct k_sem *)p3;
    event_subscription_t* sub_handle = event_bus_subscribe(my_msgq, &expected_event_id, 1);
    zassert_not_null(sub_handle, "Selective subscription failed");
    k_sem_give(my_ready_sem);
    app_event_t received_event;
    int ret = k_msgq_get(my_msgq, &received_event, K_MSEC(1000));
    zassert_equal(ret, 0, "Selective subscriber failed to receive event");
    zassert_equal(received_event.id, expected_event_id, "Received wrong event ID");
    k_sem_give(&test_done_sem);
}

void publisher_thread_entry(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p2); ARG_UNUSED(p3);
    app_event_t *event_to_send = (app_event_t *)p1;
	zassert_ok(event_bus_publish(event_to_send), "Publisher thread failed to send event");
	k_sem_give(&test_done_sem);
}

void stress_publisher_thread_entry(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);
    app_event_t event = { .id = EVENT_MOTOR_SPEED_REPORT };
    for (int i = 0; i < STRESS_TEST_EVENT_COUNT; i++) {
        event.payload.u32 = i;
        event_bus_publish(&event);
        k_yield();
    }
    k_sem_give(&test_done_sem);
}

static volatile int stress_test_received_count = 0;
void stress_subscriber_thread_entry(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);
    const event_id_t event_id = EVENT_MOTOR_SPEED_REPORT;
    event_bus_subscribe(&stress_sub_msgq, &event_id, 1);
    k_sem_give(&sub1_ready_sem);

    app_event_t received_event;
    for (int i = 0; i < STRESS_TEST_EVENT_COUNT; i++) {
        if (k_msgq_get(&stress_sub_msgq, &received_event, K_MSEC(500)) == 0) {
            stress_test_received_count++;
        } else {
            break; // Stop counting if we time out
        }
    }
    k_sem_give(&test_done_sem);
}

void high_prio_subscriber_thread_entry(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);
    const event_id_t event = EVENT_UI_BUTTON_START_PRESSED;
    event_bus_subscribe(&single_sub_msgq, &event, 1);
    k_sem_give(&sub1_ready_sem);
    app_event_t received_event;
    k_msgq_get(&single_sub_msgq, &received_event, K_FOREVER);
    k_sem_give(&priority_test_sem);
}

// --- ZTest Setup and Teardown ---

static void event_bus_before(void *data)
{
    ARG_UNUSED(data);
    event_bus_init();
}

static void event_bus_after(void *data)
{
    ARG_UNUSED(data);
    if (sub1_thread_started) { k_thread_abort(&sub1_thread_data); sub1_thread_started = false; }
    if (sub2_thread_started) { k_thread_abort(&sub2_thread_data); sub2_thread_started = false; }
    if (pub1_thread_started) { k_thread_abort(&pub1_thread_data); pub1_thread_started = false; }
    if (pub2_thread_started) { k_thread_abort(&pub2_thread_data); pub2_thread_started = false; }
    if (stress_pub_thread_started) { k_thread_abort(&stress_pub_thread_data); stress_pub_thread_started = false; }
    if (stress_sub_thread_started) { k_thread_abort(&stress_sub_thread_data); stress_sub_thread_started = false; }

    event_bus_deinit();

    k_msgq_purge(&single_sub_msgq);
    k_msgq_purge(&multi_sub1_msgq);
    k_msgq_purge(&multi_sub2_msgq);
    k_msgq_purge(&unsub_msgq);
    k_msgq_purge(&payload_msgq);
    k_msgq_purge(&multi_pub_sub1_msgq);
    k_msgq_purge(&multi_pub_sub2_msgq);
    k_msgq_purge(&dummy_msgq_for_limit_test);
    k_msgq_purge(&stress_sub_msgq);
    k_msgq_purge(&unsub_test_msgq);

    k_sem_reset(&sub1_ready_sem);
    k_sem_reset(&sub2_ready_sem);
    k_sem_reset(&test_done_sem);
    k_sem_reset(&priority_test_sem);

    k_sleep(K_MSEC(50));
}

// --- Test Cases ---

ZTEST(event_bus_suite, test_unsubscribe_stops_events)
{
    const event_id_t event_id = EVENT_DOOR_UNLOCKED;
    app_event_t event = { .id = event_id };
    event_subscription_t* sub_handle = event_bus_subscribe(&unsub_test_msgq, &event_id, 1);
    zassert_not_null(sub_handle, "Subscription failed");
    zassert_ok(event_bus_publish(&event), "First publish failed");
    app_event_t received_event;
    int ret = k_msgq_get(&unsub_test_msgq, &received_event, K_MSEC(200));
    zassert_equal(ret, 0, "Should have received event before unsubscribing");
    zassert_ok(event_bus_unsubscribe(sub_handle), "Unsubscribe failed");
    zassert_ok(event_bus_publish(&event), "Second publish failed");
    ret = k_msgq_get(&unsub_test_msgq, &received_event, K_MSEC(200));
    zassert_equal(ret, -EAGAIN, "Should not have received event after unsubscribing");
}

ZTEST(event_bus_suite, test_high_frequency_publish)
{
    stress_test_received_count = 0;

    // Create concurrent subscriber and publisher threads
    k_thread_create(&stress_sub_thread_data, stress_sub_stack, K_THREAD_STACK_SIZEOF(stress_sub_stack),
                    stress_subscriber_thread_entry, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(7), 0, K_NO_WAIT);
    stress_sub_thread_started = true;

    k_thread_create(&stress_pub_thread_data, stress_pub_stack, K_THREAD_STACK_SIZEOF(stress_pub_stack),
                    stress_publisher_thread_entry, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(7), 0, K_NO_WAIT);
    stress_pub_thread_started = true;
    
    // Wait for the subscriber to be ready before the publisher starts in earnest
    zassert_ok(k_sem_take(&sub1_ready_sem, K_MSEC(500)), "Stress subscriber never became ready");
    
    // Wait for both threads to finish their work
    zassert_ok(k_sem_take(&test_done_sem, K_MSEC(3000)), "Stress publisher did not finish");
    zassert_ok(k_sem_take(&test_done_sem, K_MSEC(3000)), "Stress subscriber did not finish");

    zassert_equal(stress_test_received_count, STRESS_TEST_EVENT_COUNT, "Did not receive all events from stress test. Expected %d, got %d", STRESS_TEST_EVENT_COUNT, stress_test_received_count);
}

ZTEST(event_bus_suite, test_high_priority_subscriber)
{
    k_thread_create(&sub1_thread_data, sub1_stack, K_THREAD_STACK_SIZEOF(sub1_stack),
                    high_prio_subscriber_thread_entry, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(6), 0, K_NO_WAIT);
    sub1_thread_started = true;
    
    zassert_ok(k_sem_take(&sub1_ready_sem, K_MSEC(500)), "High prio sub never became ready");

    k_thread_priority_set(k_current_get(), K_PRIO_PREEMPT(7));
    
    app_event_t event = { .id = EVENT_UI_BUTTON_START_PRESSED };
    event_bus_publish(&event);

    int ret = k_sem_take(&priority_test_sem, K_NO_WAIT);
    zassert_equal(ret, 0, "High priority thread did not run immediately");
}

ZTEST(event_bus_suite, test_subscription_limit)
{
    const event_id_t dummy_event = EVENT_UI_BUTTON_PAUSE_PRESSED;
    for (int i = 0; i < MAX_SUBSCRIPTIONS; i++) {
        event_subscription_t* sub_handle = event_bus_subscribe(&dummy_msgq_for_limit_test, &dummy_event, 1);
        zassert_not_null(sub_handle, "Subscription %d should have succeeded", i);
    }
    event_subscription_t* failing_sub = event_bus_subscribe(&dummy_msgq_for_limit_test, &dummy_event, 1);
    zassert_is_null(failing_sub, "Subscription should fail when pool is full");
}

ZTEST(event_bus_suite, test_single_publish_subscribe)
{
    app_event_t event_to_pub = { .id = EVENT_UI_BUTTON_START_PRESSED, .payload.s32 = 12345 };
    const event_id_t events[] = { EVENT_UI_BUTTON_START_PRESSED };
    event_subscription_t* sub_handle = event_bus_subscribe(&single_sub_msgq, events, ARRAY_SIZE(events));
    zassert_not_null(sub_handle, "Subscription failed");
    int ret = event_bus_publish(&event_to_pub);
    zassert_equal(ret, 0, "Publish failed");
    app_event_t received_event;
    ret = k_msgq_get(&single_sub_msgq, &received_event, K_MSEC(200));
    zassert_equal(ret, 0, "Failed to receive published event");
    zassert_equal(received_event.id, event_to_pub.id, "Event ID mismatch");
    zassert_equal(received_event.payload.s32, event_to_pub.payload.s32, "Payload mismatch");
}

ZTEST(event_bus_suite, test_one_to_many_dispatch)
{
    const event_id_t events[] = { EVENT_DOOR_LOCKED };
    k_thread_create(&sub1_thread_data, sub1_stack, K_THREAD_STACK_SIZEOF(sub1_stack),
                    generic_subscriber_thread_entry, &multi_sub1_msgq, (void*)events, (void*)((size_t)ARRAY_SIZE(events)),
                    K_PRIO_PREEMPT(7), 0, K_NO_WAIT);
    sub1_thread_started = true;
    k_thread_create(&sub2_thread_data, sub2_stack, K_THREAD_STACK_SIZEOF(sub2_stack),
                    generic_subscriber_thread_entry, &multi_sub2_msgq, (void*)events, (void*)((size_t)ARRAY_SIZE(events)),
                    K_PRIO_PREEMPT(7), 0, K_NO_WAIT);
    sub2_thread_started = true;
    zassert_ok(k_sem_take(&sub1_ready_sem, K_MSEC(500)));
    zassert_ok(k_sem_take(&sub2_ready_sem, K_MSEC(500)));
    app_event_t event_to_pub = { .id = EVENT_DOOR_LOCKED };
    zassert_ok(event_bus_publish(&event_to_pub));
    zassert_ok(k_sem_take(&test_done_sem, K_MSEC(500)));
    zassert_ok(k_sem_take(&test_done_sem, K_MSEC(500)));
}

ZTEST(event_bus_suite, test_multiple_publishers_and_subscribers)
{
	app_event_t door_event = { .id = EVENT_DOOR_LOCKED };
	app_event_t heater_event = { .id = EVENT_HEATER_TEMP_CHANGED };
	k_thread_create(&sub1_thread_data, sub1_stack, K_THREAD_STACK_SIZEOF(sub1_stack),
			selective_subscriber_thread_entry, &multi_pub_sub1_msgq, (void*)EVENT_DOOR_LOCKED, &sub1_ready_sem,
			K_PRIO_PREEMPT(7), 0, K_NO_WAIT);
	sub1_thread_started = true;
	k_thread_create(&sub2_thread_data, sub2_stack, K_THREAD_STACK_SIZEOF(sub2_stack),
			selective_subscriber_thread_entry, &multi_pub_sub2_msgq, (void*)EVENT_HEATER_TEMP_CHANGED, &sub2_ready_sem,
			K_PRIO_PREEMPT(7), 0, K_NO_WAIT);
	sub2_thread_started = true;
	zassert_ok(k_sem_take(&sub1_ready_sem, K_MSEC(500)));
	zassert_ok(k_sem_take(&sub2_ready_sem, K_MSEC(500)));
	k_thread_create(&pub1_thread_data, pub1_stack, K_THREAD_STACK_SIZEOF(pub1_stack),
			publisher_thread_entry, &door_event, NULL, NULL,
			K_PRIO_PREEMPT(7), 0, K_NO_WAIT);
	pub1_thread_started = true;
	k_thread_create(&pub2_thread_data, pub2_stack, K_THREAD_STACK_SIZEOF(pub2_stack),
			publisher_thread_entry, &heater_event, NULL, NULL,
			K_PRIO_PREEMPT(7), 0, K_NO_WAIT);
	pub2_thread_started = true;
	zassert_ok(k_sem_take(&test_done_sem, K_MSEC(500)));
	zassert_ok(k_sem_take(&test_done_sem, K_MSEC(500)));
	zassert_ok(k_sem_take(&test_done_sem, K_MSEC(500)));
	zassert_ok(k_sem_take(&test_done_sem, K_MSEC(500)));
}

ZTEST(event_bus_suite, test_unsubscribed_event_is_not_received)
{
    const event_id_t events[] = { COMMAND_MOTOR_SET_SPEED };
    event_subscription_t* sub_handle = event_bus_subscribe(&unsub_msgq, events, ARRAY_SIZE(events));
    zassert_not_null(sub_handle, "Subscription failed");
    app_event_t event_to_pub = { .id = EVENT_HEATER_TEMP_CHANGED };
    zassert_ok(event_bus_publish(&event_to_pub));
    app_event_t received_event;
    int ret = k_msgq_get(&unsub_msgq, &received_event, K_MSEC(200));
    zassert_equal(ret, -EAGAIN, "Should not have received an unsubscribed event");
}

ZTEST(event_bus_suite, test_null_args)
{
    const event_id_t events[] = { COMMAND_DOOR_SET_LOCK };
    zassert_equal(event_bus_publish(NULL), -EINVAL);
    zassert_is_null(event_bus_subscribe(NULL, events, 1));
    zassert_is_null(event_bus_subscribe(&single_sub_msgq, NULL, 1));
    zassert_is_null(event_bus_subscribe(&single_sub_msgq, events, 0));
}

ZTEST(event_bus_suite, test_payload_integrity)
{
    const event_id_t events[] = { COMMAND_HEATER_SET_TEMP, COMMAND_DOOR_SET_LOCK };
    event_subscription_t* sub_handle = event_bus_subscribe(&payload_msgq, events, ARRAY_SIZE(events));
    zassert_not_null(sub_handle, "Subscription failed");
    app_event_t float_event = { .id = COMMAND_HEATER_SET_TEMP, .payload.f = 95.5f };
    zassert_ok(event_bus_publish(&float_event));
    app_event_t received_event;
    zassert_ok(k_msgq_get(&payload_msgq, &received_event, K_MSEC(200)));
    zassert_equal(received_event.id, float_event.id);
    zassert_within(received_event.payload.f, float_event.payload.f, 0.01f);
    app_event_t bool_event = { .id = COMMAND_DOOR_SET_LOCK, .payload.b = true };
    zassert_ok(event_bus_publish(&bool_event));
    zassert_ok(k_msgq_get(&payload_msgq, &received_event, K_MSEC(200)));
    zassert_equal(received_event.id, bool_event.id);
    zassert_true(received_event.payload.b);
}

ZTEST(event_bus_suite, test_publish_with_no_subscribers)
{
    app_event_t event = { .id = EVENT_MOTOR_STOPPED };
    zassert_ok(event_bus_publish(&event));
}

ZTEST(event_bus_suite, test_publish_with_invalid_event_id)
{
	app_event_t event = { .id = EVENT_ID_COUNT };
	zassert_equal(event_bus_publish(&event), -EINVAL);

    app_event_t event2 = { .id = 9999 };
	zassert_equal(event_bus_publish(&event2), -EINVAL);
}

// --- Test Suite Definition ---

ZTEST_SUITE(event_bus_suite, NULL, NULL, event_bus_before, event_bus_after, NULL);

