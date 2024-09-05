#ifndef GRAVITY_H
#define GRAVITY_H

/* ********************** ESP32 Gravity ***********************
 * Gravity started life as a way to learn how to assemble and *
 * send wireless packets. I started with the 'RickRoll' beacon*
 * attack, and continued the theme.                           *
 * I like to think that Gravity differentiates itself from    *
 * Marauder in that Marauder feels to me like it's about      *
 * gathering information - scanning, capturing handshakes,    *
 * etc. - whereas Gravity is all about sending stuff down the *
 * wire. I'm fascinated by the possibilities of combining Mana*
 * with other attacks, so my initial drive was to develop a   *
 * Mana process from the ground up.                           *
 *                                                            *
 * Flipper-Gravity, a companion Flipper Zero app, can be      *
 * downloaded from https://github.com/chris-bc/Flipper-Gravity*
 *                                                            *
 * ESP32-Gravity: https://github.com/chris-bc/esp32-gravity   *
 *                                                            *
 * Licensed under the MIT Open Source License.                *
 **************************************************************/
#define GRAVITY_VERSION "1.0.1"
#define MAX_16_BIT 65535
#define MAX_SSID_LEN 32
#define MAC_STRLEN 17

#include <cmd_nvs.h>
#include <cmd_system.h>
#include <cmd_wifi.h>
#include <esp_console.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_system.h>
#include <esp_vfs_dev.h>
#include <esp_vfs_fat.h>
#include <esp_wifi.h>
#include <esp_wifi_types.h>
#include <freertos/portmacro.h>
#include <nvs.h>
#include <nvs_flash.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

/* Command specifications */
esp_err_t cmd_bluetooth(int argc, char **argv);
esp_err_t cmd_version(int argc, char **argv);

void initPromiscuous();
int initialise_wifi();

bool WIFI_INITIALISED = false;

#define CMD_COUNT 2
esp_console_cmd_t commands[CMD_COUNT] = {
    {
        .command = "bluetooth",
        .hint = "bluetooth hint",
        .help = "Bluetooth help",
        .func = cmd_bluetooth
    }, {
        .command = "gravity-version",
        .hint = "version hint",
        .help = "Display esp32-Gravity version information.",
        .func = cmd_version
    }
};

#endif
