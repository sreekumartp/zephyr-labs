#ifndef MAIN_H
#define MAIN_H

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
// Add your declarations here

bool is_door_closed(void);

#endif // MAIN_H