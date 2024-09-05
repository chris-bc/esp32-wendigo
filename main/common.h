#ifndef GRAVITY_COMMON_H
#define GRAVITY_COMMON_H

/* Macro to mark a variable as unused in order to prevent compiler warnings
   This is defined in several of the bluetooth header files, but Bluetooth
   may not be present in the current chipset */
#ifndef UNUSED
    #define UNUSED(x) (void)(x)
#endif

#include <stdbool.h>

#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_interface.h>
#include <esp_wifi_types.h>

/* Adding mana.h causes it to be unabe to use PROBE_RESPONSE_AUTH_TYPE */
/* Adding scan.h causes it to be unable to use ScanResultAP and ScanResultSTA */
#include "gravity.h"

char STRINGS_HOP_STATE_FAIL[];
char STRINGS_MALLOC_FAIL[];
char STRINGS_SET_MAC_FAIL[];
char STRINGS_HOPMODE_INVALID[];

esp_err_t bytes_to_string(uint8_t *bytes, char *string, int byteCount);
esp_err_t mac_bytes_to_string(uint8_t *bMac, char *strMac);

#endif