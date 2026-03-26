#pragma once

/**
 * Socket configuration for multicast communication,
 * should be the same for receiver and sender.
 * Set through config menu
 * 
 * note: MULTICAST_GROUP_IP set according the the following range
 * 
 * 239.0.0.0 ~ 239.255.255.255: multicast addresses for local management, 
 * which are valid only within specific local ranges.
 */

#include "esp_wifi.h"

#define MULTICAST_GROUP_IP CONFIG_MULTICAST_GROUP_IP //"239.0.0.1"
#define MULTICAST_PORT     CONFIG_MULTICAST_PORT     //4444
#define MULTICAST_TTL      CONFIG_MULTICAST_TTL      //1

typedef void (*socket_command_callback_t)(int command[], int size);

esp_err_t esp_recv_multicast(socket_command_callback_t command_callback);
