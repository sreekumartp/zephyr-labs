/*
 * Copyright (c) 2024 Your Name
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SIM_DOOR_SENSOR_H_
#define SIM_DOOR_SENSOR_H_

#include <stdbool.h>

/**
 * @brief Initializes the simulated door sensor component.
 *
 * This function gets the required device handles from the device tree
 * and configures the GPIO pin for the sensor. It must be called
 * before any other function in this component.
 *
 * @retval 0 if successful.
 * @retval -ENODEV if a required device is not ready.
 * @retval other negative error code on failure.
 */
int door_sensor_sim_init(void);

/**
 * @brief Sets the state of the simulated door sensor.
 *
 * This function tells the GPIO emulator to set the physical state
 * of the sensor's GPIO pin.
 *
 * @param closed True to set the sensor state to "closed" (active/high),
 * false to set it to "open" (inactive/low).
 *
 * @retval 0 if successful.
 * @retval other negative error code on failure.
 */
int door_sensor_sim_set_state(bool closed);

/**
 * @brief Gets the current state of the simulated door sensor.
 *
 * Reads the current logical value of the sensor's GPIO pin.
 *
 * @return True if the sensor is "closed" (pin is high), false otherwise.
 */
bool door_sensor_sim_get_state(void);


#endif /* SIM_DOOR_SENSOR_H_ */
