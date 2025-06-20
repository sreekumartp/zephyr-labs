#include <zephyr/shell/shell.h>
#include "sim_door_sensor.h"
#include "sim_water_level.h"

static int cmd_door(const struct shell *sh, size_t argc, char **argv){
    door_sensor_sim_set_state(strcmp(argv[1],"close")==0);
    shell_print(sh, "Door state: %s", argv[1]);
    return 0;
}
SHELL_CMD_ARG_REGISTER(door, NULL, "door open|close", cmd_door, 2, 0);

static int cmd_water(const struct shell *sh, size_t argc, char **argv){
    water_level_sim_set_state(strcmp(argv[1],"full")==0);
    shell_print(sh, "Water level: %s", argv[1]);
    return 0;
}
SHELL_CMD_ARG_REGISTER(water, NULL, "water empty|full", cmd_water, 2, 0);

void wm_shell_init(void) { /* Trigger shell if needed */ }
