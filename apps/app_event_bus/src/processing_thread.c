#include "processing_thread.h" // <-- Include the new header
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(processing, CONFIG_LOG_DEFAULT_LEVEL);

// This is now the global definition of the message queue.
// It matches the 'extern' declaration in the header.
K_MSGQ_DEFINE(processing_msgq, sizeof(app_event_t), 10, 4);

static void processing_thread_entry(void)
{
    LOG_INF("Processing thread started, waiting for messages.");
    app_event_t received_event;

    while (1) {
        // Wait forever for a message to arrive
        k_msgq_get(&processing_msgq, &received_event, K_FOREVER);

        LOG_INF("Processing thread DEQUEUED event %d", received_event.id);

        // --- HEAVY OR BLOCKING PROCESSING HAPPENS HERE ---
        k_msleep(500);

        LOG_INF("Processing thread FINISHED handling event. Payload: %d", received_event.payload.s32);
    }
}

K_THREAD_DEFINE(processing_thread, 1024, processing_thread_entry, NULL, NULL, NULL, 7, 0, 0);