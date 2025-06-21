#include <zephyr/shell/shell.h>
#include <errno.h>
#include "sim_door_sensor.h"
#include "sim_water_level.h"
static int cmd_door(const struct shell *sh, size_t argc, char **argv) {
    if (strcmp(argv[1], "close") == 0) {
        door_sensor_sim_set_state(true);
        shell_print(sh, "Door state: closed");
    } else if (strcmp(argv[1], "open") == 0) {
        door_sensor_sim_set_state(false);
        shell_print(sh, "Door state: open");
    } else {
        shell_print(sh, "❌ Invalid argument. Use 'open' or 'close'");
        return -EINVAL;
    }
    return 0;
}
SHELL_CMD_ARG_REGISTER(door, NULL, "door open|close", cmd_door, 2, 0);

static int cmd_water(const struct shell *sh, size_t argc, char **argv) {
    if (strcmp(argv[1], "full") == 0) {
        water_level_sim_set_state(true);
        shell_print(sh, "Water level: full");
    } else if (strcmp(argv[1], "empty") == 0) {
        water_level_sim_set_state(false);
        shell_print(sh, "Water level: empty");
    } else {
        shell_print(sh, "❌ Invalid argument. Use 'empty' or 'full'");
        return -EINVAL;
    }
    return 0;
}
SHELL_CMD_ARG_REGISTER(water, NULL, "water empty|full", cmd_water, 2, 0);

static int cmd_door_state(const struct shell *sh, size_t argc, char **argv){
    bool state = door_sensor_sim_get_state();
    shell_print(sh, "Door is currently: %s", state ? "closed" : "open");
    return 0;
}
SHELL_CMD_REGISTER(door_state, NULL, "Get current door state", cmd_door_state);

static int cmd_water_state(const struct shell *sh, size_t argc, char **argv){
    bool state = water_level_sim_get_state();
    shell_print(sh, "Water level is currently: %s", state ? "full" : "empty");
    return 0;
}
SHELL_CMD_REGISTER(water_state, NULL, "Get current water level", cmd_water_state);

void wm_shell_init(void) { /* Trigger shell if needed */ }
