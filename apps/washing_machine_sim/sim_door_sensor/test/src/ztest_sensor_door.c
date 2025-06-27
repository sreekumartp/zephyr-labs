#include <zephyr/ztest.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/time_units.h>

#include "event_bus.h"
#include "event_defs.h"

LOG_MODULE_REGISTER(test_sensor_door, LOG_LEVEL_INF);

extern int sensor_door_start(void);
extern event_id_t sensor_door_get_state(void);
static struct {
    app_event_t event;
    int count;
    int64_t timestamps[10];
} door_event_log;

static struct k_sem event_sem;

static void test_event_handler(const app_event_t *event) {
    LOG_INF("test_event_handler received event id: %d", event->id);

    if (event->id == EVENT_DOOR_OPENED || event->id == EVENT_DOOR_CLOSED) {
        if (door_event_log.count < 10) {
            door_event_log.timestamps[door_event_log.count] = k_uptime_get();
        }
        door_event_log.event = *event;
        door_event_log.count++;
        k_sem_give(&event_sem);
    }
}

static void *setup_fn(void)
{
    LOG_INF("Initializing event bus and test environment");
    event_bus_init();
    k_sem_init(&event_sem, 0, 10);
    door_event_log.count = 0;
    event_bus_register_handler(test_event_handler);
    sensor_door_start();
    return NULL;
}

ZTEST(sensor_door_tests, test_door_event_published_on_state_change)
{
    event_id_t door_open = EVENT_DOOR_OPENED;
    app_event_t inject = { .id = EVENT_UNKNOWN };
    memcpy(&inject.id, &door_open, sizeof(door_open));
    LOG_INF("Publishing EVENT_DOOR_OPENED with door_open: %d", door_open);
    event_bus_publish(&inject);
    k_sleep(K_SECONDS(1));
    event_id_t retrieved_state=sensor_door_get_state();
    LOG_INF("Retrieved door state: %d", retrieved_state);

    zassert_equal(retrieved_state, EVENT_DOOR_OPENED, "Expected door state to be OPENED");

}


ZTEST(sensor_door_tests, test_door_event_published_on_state_change_closed   )
{
    event_id_t door_state = EVENT_DOOR_CLOSED;
    app_event_t inject = { .id = EVENT_UNKNOWN };
    memcpy(&inject.id, &door_state, sizeof(door_state));
    LOG_INF("Publishing EVENT_DOOR_CLOSED with door_open: %d", door_state);
    event_bus_publish(&inject);
    k_sleep(K_SECONDS(1));
    event_id_t retrieved_state=sensor_door_get_state();
    LOG_INF("Retrieved door state: %d", retrieved_state);

    zassert_equal(retrieved_state, EVENT_DOOR_CLOSED, "Expected door state to be door_state CLOSED");


}


//simulates a faulty door sensor that toggles between open and closed states
// this is to test the robustness of the door sensor handling logic
// it should not cause the system to crash or behave unexpectedly
ZTEST(sensor_door_tests, test_door_event_toggles)
{
    
    event_id_t door_state = EVENT_DOOR_CLOSED;
    app_event_t evt = { .id = EVENT_UNKNOWN};


    for(int i=0; i < 3; i++) {
  
        door_state = EVENT_DOOR_OPENED;
        memcpy(&evt.id, &door_state, sizeof(door_state));
        LOG_INF("Publishing EVENT_TEST_DOOR_INPUT with door_closed: %d", door_state);
        event_bus_publish(&evt);
        
        k_sleep(K_MSEC(10));
        door_state = EVENT_DOOR_CLOSED;
        memcpy(&evt.id, &door_state, sizeof(door_state));
        LOG_INF("Publishing EVENT_TEST_DOOR_INPUT with door_open: %d", door_state);
        event_bus_publish(&evt);
        k_sleep(K_MSEC(10));

    }
    event_id_t retrieved_state=sensor_door_get_state();
    LOG_INF("Retrieved door state: %d", retrieved_state);

    zassert_equal(retrieved_state, EVENT_DOOR_CLOSED, "Expected door state to be door_state CLOSED");
}


ZTEST_SUITE(sensor_door_tests, NULL, setup_fn, NULL, NULL, NULL);
