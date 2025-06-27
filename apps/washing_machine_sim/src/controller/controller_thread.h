#ifndef CONTROLLER_THREAD_H
#define CONTROLLER_THREAD_H

#include <zephyr/kernel.h>
#include "fsm.h"
#include "event_bus.h"

// Declare the message queue as 'extern' so the callback can access it.
extern struct k_msgq fsm_msgq;

/**
 * @brief Initializes and starts the FSM controller thread.
 *
 * This function subscribes to all relevant FSM events and creates the
 * controller thread that will process them. It should be called once
 * during system startup.
 *
 * @retval 0 on success
 * @retval -1 on failure
 */
int controller_thread_init(void);

/**
 * @brief Gets a handle to the main FSM structure.
 *
 * @return A pointer to the fsm_handle_t instance.
 */
fsm_handle_t *controller_fsm_get_handle(void);

#endif // CONTROLLER_THREAD_H