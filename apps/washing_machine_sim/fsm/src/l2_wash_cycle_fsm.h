#pragma once

#include "fsm.h" // Includes the public API definitions (states, events, handle)

/**
 * @brief Initializes the L2 Wash Cycle FSM to its default IDLE state.
 *
 * This function is called by the L1 FSM when a cycle is canceled or
 * when the system initializes.
 *
 * @param fsm A pointer to the FSM handle.
 */
void l2_fsm_init(fsm_handle_t *fsm);

/**
 * @brief Starts the L2 Wash Cycle FSM.
 *
 * This transitions the L2 FSM from IDLE to its first active state,
 * which is LOAD_SENSING. It is called by the L1 FSM when it enters
 * the RUNNING state.
 *
 * @param fsm A pointer to the FSM handle.
 */
void l2_fsm_start(fsm_handle_t *fsm);

/**
 * @brief Processes an event for the L2 Wash Cycle FSM.
 *
 * This function is called by the main dispatcher when the system is in the
 * RUNNING state and an event is not a high-priority L1 override. It handles
 * the detailed steps of the wash cycle, such as filling, washing, and spinning.
 *
 * @param fsm A pointer to the FSM handle.
 * @param event The event to process.
 */
void l2_wash_cycle_process_event(fsm_handle_t *fsm, event_id_t event);
