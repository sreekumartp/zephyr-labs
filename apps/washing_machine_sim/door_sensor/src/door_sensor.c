#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/__assert.h>
#include <zephyr/logging/log.h>
#include "sim_door_sensor.h"
#include "event_bus.h"
#include "event_defs.h"
#include "door_sensor.h"

LOG_MODULE_REGISTER(door_sensor, LOG_LEVEL_INF);
#define DOOR_SENSOR_POLL_INTERVAL_MS 100

static bool last_state;

static void door_sensor_poll_thread(void *arg1, void *arg2, void *arg3)
{
    ARG_UNUSED(arg1);
    ARG_UNUSED(arg2);
    ARG_UNUSED(arg3);
    event_id_t evt;

    while (1) {
        bool current = door_sensor_sim_get_state();
        if (current != last_state) {
            last_state = current;
            if (current == 0)
            {
                evt = EVENT_DOOR_OPENED;
            }
            else
            {
                evt = EVENT_DOOR_CLOSED;
            }
            app_event_t event = {
                .id = evt,
                .sender_tid = k_current_get(),
                .payload.u32 = k_uptime_get_32()
            };

            LOG_INF("door_sensor_poll_thread posting event ID: %d", evt);

            event_bus_post(&event);
        }
        k_msleep(DOOR_SENSOR_POLL_INTERVAL_MS);
    }
}

K_THREAD_STACK_DEFINE(door_sensor_stack, 512);
static struct k_thread door_sensor_thread;

void door_sensor_init(void)
{
    bool rval=false;
    int ret;
    ret=door_sensor_sim_init();
     __ASSERT(rval == false, "Failed initializing door sensor simulation");   
    rval=door_sensor_sim_set_state(false);
    __ASSERT(rval == false, "Failed to set initial door sensor state");
    last_state = door_sensor_sim_get_state();
    k_thread_create(&door_sensor_thread, door_sensor_stack, K_THREAD_STACK_SIZEOF(door_sensor_stack),
                    door_sensor_poll_thread, NULL, NULL, NULL,
                    5, 0, K_NO_WAIT);
}