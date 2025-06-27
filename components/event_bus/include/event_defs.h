#pragma once

#include <zephyr/kernel.h>
#include <stdint.h>
#include <stdbool.h>


typedef enum {
    COMMAND_DOOR_SET_LOCK=0,          // Payload: true for lock, false for unlock
    COMMAND_HEATER_SET_TEMP,        // Payload: target_temp_celsius
    COMMAND_MOTOR_SET_SPEED,        // Payload: target_rpm

    EVENT_ANY_KEY_PRESSED,
    EVENT_APP_MESSAGE_SENT,         // Payload: message_id (int32_t)
    EVENT_CANCEL_BUTTON_PRESSED,
    EVENT_CYCLE_FINISHED,
    EVENT_CYCLE_SELECTED,
    EVENT_DOOR_CLOSED,
    EVENT_DOOR_LOCKED,
    EVENT_DOOR_OPENED,
    EVENT_DOOR_UNLOCKED,
    EVENT_DRUM_EMPTY,
    EVENT_DOSING_COMPLETE,
    EVENT_FATAL_FAULT_DETECTED,
    EVENT_HEATER_TEMP_CHANGED,      // Payload: current_temp_celsius
    EVENT_MOTOR_SPEED_REPORT,       // Payload: current_rpm
    EVENT_MOTOR_STOPPED,
    EVENT_PAUSE_BUTTON_PRESSED,
    EVENT_POWER_BUTTON_PRESSED,
    EVENT_POWER_LOSS_DETECTED,
    EVENT_POWER_RESTORED,
    EVENT_START_BUTTON_PRESSED,
    EVENT_STEAM_COMPLETE,
    EVENT_STEAM_READY,
    EVENT_TEMP_REACHED,
    EVENT_TEST_DOOR_INPUT,
    EVENT_TIMER_EXPIRED,
    EVENT_UI_BUTTON_PAUSE_PRESSED,
    EVENT_UI_BUTTON_START_PRESSED,
    EVENT_UI_CYCLE_SELECTED,        // Payload: cycle_id
    EVENT_UNKNOWN,
    EVENT_WATER_LEVEL_CHANGED,      // Payload: current_level_mm
    EVENT_WATER_LEVEL_REACHED,
    EVENT_WEIGHT_CALCULATED,
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
