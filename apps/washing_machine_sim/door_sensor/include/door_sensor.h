#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the door sensor wrapper.
 *
 * This function starts the background polling for the simulated door sensor.
 * It will post an EVENT_DOOR_STATE_CHANGED event on state changes.
 */
void door_sensor_init(void);

#ifdef __cplusplus
}
#endif