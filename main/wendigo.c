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

#include "wendigo.h"

#include "common.h"
#include "esp_err.h"
#include "esp_wifi_types.h"
#include "freertos/portmacro.h"
#include "sdkconfig.h"
#define PROMPT_STR "wendigo"
#define TAG "WENDIGO"

/* Console command history can be stored to and loaded from a file.
 * The easiest way to do this is to use FATFS filesystem on top of
 * wear_levelling library.
 */
#if CONFIG_CONSOLE_STORE_HISTORY

#define MOUNT_PATH "/data"
#define HISTORY_PATH MOUNT_PATH "/history.txt"

/* Over-ride the default implementation so we can send deauth frames */
esp_err_t ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3){
    return ESP_OK;
}

/* Run bluetooth test module */
/* The bluetooth command may not be needed for quite some time now it's done as a PoC
   Future expansion of the BT module to allow interactive exploration of the airspace would be neat.
*/
esp_err_t cmd_bluetooth(int argc, char **argv) {
    esp_err_t err = ESP_OK;
    #if defined(CONFIG_BT_ENABLED)
        //err |= gravity_ble_test();
        printf("BT Test harness currently inactive.\n");
    #else
        displayBluetoothUnsupported();
    #endif
    return err;
}

/* Display version info for esp32-Gravity */
esp_err_t cmd_version(int argc, char **argv) {
    esp_err_t err = ESP_OK;

    #ifdef CONFIG_FLIPPER
        printf("esp32-Gravity v%s\n", GRAVITY_VERSION);
    #else
        ESP_LOGI(TAG, "esp32-Gravity v%s\n", GRAVITY_VERSION);
    #endif
    return err;
}

/* Monitor mode callback
   This is the callback function invoked when the wireless interface receives any selected packet.
   Currently it only responds to management packets.
   This function:
    - Displays packet info when sniffing is enabled
    - Coordinates Mana probe responses when Mana is enabled
    - Invokes relevant functions to manage scan results, if scanning is enabled
*/
void wifi_pkt_rcvd(void *buf, wifi_promiscuous_pkt_type_t type) {
    wifi_promiscuous_pkt_t *data = (wifi_promiscuous_pkt_t *)buf;

  
    return;
}

int initialise_wifi() {
    /* Initialise WiFi if needed */
    if (!WIFI_INITIALISED) {
        esp_err_t ret = nvs_flash_init();
        if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
            ESP_ERROR_CHECK(nvs_flash_erase());
            ret = nvs_flash_init();
        }
        ESP_ERROR_CHECK(ret);

        ESP_ERROR_CHECK(esp_netif_init());
        ESP_ERROR_CHECK(esp_event_loop_create_default());
        esp_netif_create_default_wifi_ap();

        // Set up promiscuous mode and packet callback
        initPromiscuous();
        WIFI_INITIALISED = true;
    }
    return ESP_OK;
}

void initPromiscuous() {
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    /* Init dummy AP to specify a channel and get WiFi hardware into a
           mode where we can send the actual fake beacon frames. */
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    wifi_config_t ap_config = {
        .ap = {
            .ssid = "ManagementAP",
            .ssid_len = 12,
            .password = "management",
            .channel = 1,
            .authmode = WIFI_AUTH_OPEN,
            .ssid_hidden = 0,
            .max_connection = 128,
            .beacon_interval = 5000
        }
    };

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));

    wifi_promiscuous_filter_t filter = { .filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT | WIFI_PROMIS_FILTER_MASK_CTRL | WIFI_PROMIS_FILTER_MASK_DATA };
    esp_wifi_set_promiscuous_filter(&filter);
    esp_wifi_set_promiscuous_rx_cb(wifi_pkt_rcvd);
    esp_wifi_set_promiscuous(true);
}

static void initialize_filesystem(void)
{
    static wl_handle_t wl_handle;
    const esp_vfs_fat_mount_config_t mount_config = {
            .max_files = 4,
            .format_if_mount_failed = true
    };
    esp_err_t err = esp_vfs_fat_spiflash_mount_rw_wl(MOUNT_PATH, "storage", &mount_config, &wl_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount FATFS (%s)", esp_err_to_name(err));
        return;
    }
}
#endif // CONFIG_STORE_HISTORY

static void initialize_nvs(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK( nvs_flash_erase() );
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
}

static int register_gravity_commands() {
    esp_err_t err;
    for (int i=0; i < CMD_COUNT; ++i) {
        err = esp_console_cmd_register(&commands[i]);
        switch (err) {
        case ESP_OK:
            #ifndef CONFIG_FLIPPER
                ESP_LOGI(TAG, "Registered command \"%s\"...", commands[i].command);
            #endif
            break;
        case ESP_ERR_NO_MEM:
            ESP_LOGE(TAG, "Out of memory registering command \"%s\"!", commands[i].command);
            return ESP_ERR_NO_MEM;
        case ESP_ERR_INVALID_ARG:
            ESP_LOGW(TAG, "Invalid arguments provided during registration of \"%s\". Skipping...", commands[i].command);
            continue;
        }
    }
    return ESP_OK;
}

void app_main(void)
{
    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    /* Prompt to be printed before each line.
     * This can be customized, made dynamic, etc.
     */
    repl_config.prompt = PROMPT_STR ">";
    repl_config.max_cmdline_length = CONFIG_CONSOLE_MAX_COMMAND_LINE_LENGTH;

    initialize_nvs();

    esp_log_level_set("wifi", ESP_LOG_ERROR); /* YAGNI: Consider reducing these to ESP_LOG_WARN */
    esp_log_level_set("esp_netif_lwip", ESP_LOG_ERROR);

#if CONFIG_CONSOLE_STORE_HISTORY
    initialize_filesystem();
    repl_config.history_save_path = HISTORY_PATH;
    #ifndef CONFIG_FLIPPER
        ESP_LOGI(TAG, "Command history enabled");
    #endif
#else
    #ifndef CONFIG_FLIPPER
        ESP_LOGI(TAG, "Command history disabled");
    #endif
#endif

    initialise_wifi();
    /* Register commands */
    esp_console_register_help_command();
    register_system();
    register_wifi();
    register_gravity_commands();
    /*register_nvs();*/

    ESP_LOGI(TAG, "Started Gravity v%s\n", GRAVITY_VERSION);

#if defined(CONFIG_ESP_CONSOLE_UART_DEFAULT) || defined(CONFIG_ESP_CONSOLE_UART_CUSTOM)
    esp_console_dev_uart_config_t hw_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&hw_config, &repl_config, &repl));

#elif defined(CONFIG_ESP_CONSOLE_USB_CDC)
    esp_console_dev_usb_cdc_config_t hw_config = ESP_CONSOLE_DEV_CDC_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_usb_cdc(&hw_config, &repl_config, &repl));

#elif defined(CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG)
    esp_console_dev_usb_serial_jtag_config_t hw_config = ESP_CONSOLE_DEV_USB_SERIAL_JTAG_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_usb_serial_jtag(&hw_config, &repl_config, &repl));

#else
#error Unsupported console type
#endif

    ESP_ERROR_CHECK(esp_console_start_repl(repl));
}
