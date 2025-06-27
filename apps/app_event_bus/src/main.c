#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "event_bus.h"

LOG_MODULE_REGISTER(main, CONFIG_LOG_DEFAULT_LEVEL);

int main(void)
{
    LOG_INF("--- Event Bus Test Application ---");
    LOG_INF("Initializing event bus...");

    if (event_bus_init() != 0) {
        LOG_ERR("Event bus initialization failed!");
        return 1;
    }

    LOG_INF("Event bus initialized. Test is running...");
    return 0;
}