#include "nvs_flash.h"

#include "esp_log.h"
#include "esp_event.h"

#include "wifi_ap.h"
#include "message_handler.h"
#include "receiver_wifi_station.h"

#if CONFIG_IS_SERVER_BOARD
#define IS_SERVER_BOARD     1
#else
#define IS_SERVER_BOARD     0
#endif

static const char *TAG = "main";

void app_main(void)
{
    // initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // setup wifi ap
    if (IS_SERVER_BOARD)
    {
        ESP_LOGI(TAG, "ESP sender board init");
        wifi_init_softap_init();
        schedule_multicast();
    }
    else
    {
        ESP_LOGI(TAG, "ESP receiver board init");
        init_handler();
        init_receiver();
    }
}