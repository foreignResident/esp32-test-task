#include "message_handler.h"

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

static const char *TAG = "handler";

void init_handler() {
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL<<LED_PIN),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_ENABLE
    };
    
    gpio_config(&io_conf);

    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
}

void process_comand(int commands[], int size)
{
    ESP_LOGI(TAG, "process_comand received size %d", size);
    for(int i = 0; i < size; i++) {
        int cmd = commands[i];

        if(cmd == 0) {
            int res = gpio_set_level(LED_PIN, 0);
            ESP_LOGI(TAG, "cmd %d, LED_PIN %d res %d", cmd, LED_PIN, res);
        } else if(cmd == 1) {
            int res = gpio_set_level(LED_PIN, 1);
            ESP_LOGI(TAG, "cmd %d, LED_PIN %d res %d", cmd, LED_PIN, res);
        } else {
            // unknown command
        }

        vTaskDelay(pdMS_TO_TICKS(400));
    }
}