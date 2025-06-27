#ifndef SENSOR_DOOR_THREAD_H
#define SENSOR_DOOR_THREAD_H

void sensor_door_thread(void *, void *, void *);
int sensor_door_start(void);
event_id_t sensor_door_get_state(void);

#endif // SENSOR_DOOR_THREAD_H
