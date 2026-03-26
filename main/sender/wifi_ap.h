#pragma once

#include "esp_event.h"

/**
 * WiFi configuration for ap. Set through config menu
 */
#define ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD
#define ESP_WIFI_CHANNEL   CONFIG_ESP_WIFI_CHANNEL
#define MAX_STA_CONN       CONFIG_ESP_MAX_STA_CONN

void wifi_init_softap_init(void);
void schedule_multicast();