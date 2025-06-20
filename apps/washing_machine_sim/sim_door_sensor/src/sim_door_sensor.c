/*
 * Copyright (c) 2024 Your Name
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/gpio/gpio_emul.h>

#include "sim_door_sensor.h"

#if !DT_HAS_ALIAS(sensor0)
#error "The devicetree must have a 'sensor0' alias."
#endif

static struct gpio_dt_spec sensor;
static const struct device *gpio_emul_dev;

int door_sensor_sim_init(void)
{
	int ret;

	sensor = (struct gpio_dt_spec)GPIO_DT_SPEC_GET(DT_ALIAS(sensor0), gpios);
	gpio_emul_dev = DEVICE_DT_GET(DT_NODELABEL(gpio0));

	if (!device_is_ready(gpio_emul_dev)) {
		return -ENODEV;
	}

	if (!device_is_ready(sensor.port)) {
		return -ENODEV;
	}

	/*
	 * ====================================================================
	 * FIX: Configure the pin as an input within the component's init function.
	 * This ensures the component is self-contained and ready to be used
	 * immediately after initialization.
	 * ====================================================================
	 */
	ret = gpio_pin_configure_dt(&sensor, GPIO_INPUT);
	if (ret != 0) {
		return ret;
	}

	return 0;
}

int door_sensor_sim_set_state(bool closed)
{
	int state = closed ? 1 : 0;
	return gpio_emul_input_set(gpio_emul_dev, sensor.pin, state);
}

bool door_sensor_sim_get_state(void)
{
	int val = gpio_pin_get_dt(&sensor);
	return val > 0;
}
