#pragma once

#include <zephyr/kernel.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    // --- Events from UI to Controller ---
    EVENT_UI_BUTTON_START_PRESSED,
    EVENT_UI_BUTTON_PAUSE_PRESSED,
    EVENT_UI_CYCLE_SELECTED,        // Payload: cycle_id

    // --- Events from Door Sensor to Controller ---
    EVENT_DOOR_LOCKED,
    EVENT_DOOR_UNLOCKED,

    // --- Events from Water Level Sensor to Controller ---
    EVENT_WATER_LEVEL_CHANGED,      // Payload: current_level_mm

    // --- Events from Motor to Controller ---
    EVENT_MOTOR_SPEED_REPORT,       // Payload: current_rpm
    EVENT_MOTOR_STOPPED,

    // --- Events from Heater to Controller ---
    EVENT_HEATER_TEMP_CHANGED,      // Payload: current_temp_celsius

    // --- Commands from Controller to Components ---
    COMMAND_MOTOR_SET_SPEED,        // Payload: target_rpm
    COMMAND_HEATER_SET_TEMP,        // Payload: target_temp_celsius
    COMMAND_DOOR_SET_LOCK,          // Payload: true for lock, false for unlock

    // --- This must be the last entry ---
    EVENT_ID_COUNT
} event_id_t;

typedef union {
    uint32_t u32;
    int32_t  s32;
    bool     b;
    float    f;
} event_payload_t;


typedef struct {
    event_id_t      id;
    k_tid_t         sender_tid;
    event_payload_t payload;
} app_event_t;
