#pragma once

#include <zephyr/kernel.h>
#include "event_bus.h" // For app_event_t

// Declare the message queue as 'extern' so other files know it exists globally.
extern struct k_msgq processing_msgq;