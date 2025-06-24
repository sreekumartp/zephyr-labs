#pragma once

#include "fsm.h" // Includes the public API definitions (states, events, handle)

/**
 * @brief Initializes the L1 System FSM to its default starting state.
 *
 * @param fsm A pointer to the FSM handle.
 */
void l1_fsm_init(fsm_handle_t *fsm);

/**
 * @brief Processes high-priority events that can override the L2 FSM.
 *
 * This function checks for events like PAUSE, POWER_LOSS, or FATAL_FAULT
 * that must be handled immediately, even when the L2 wash cycle is active.
 *
 * @param fsm A pointer to the FSM handle.
 * @param event The event to process.
 * @return True if the event was handled as an override, false otherwise.
 */
bool l1_system_process_override_events(fsm_handle_t *fsm, fsm_event_t event);

/**
 * @brief Processes a standard event for the L1 System FSM.
 *
 * This function is called by the main dispatcher when the system is not in the
 * RUNNING state, or when an event is not a high-priority override.
 *
 * @param fsm A pointer to the FSM handle.
 * @param event The event to process.
 */
void l1_system_process_event(fsm_handle_t *fsm, fsm_event_t event);
