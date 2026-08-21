#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "hal/gpio_types.h"

#define LED_GPIO 2

static const char *TAG = "ESP9";


void app_main(void) {
    gpio_reset_pin(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

    ESP_LOGI(TAG, "Waiting for your input...");

    while(1) {
        int User_num = getchar();

        if (User_num == '1') {
            gpio_set_level(LED_GPIO, 1);
            ESP_LOGI(TAG, "LED ON");
        }
        else if (User_num == '0') {
            gpio_set_level(LED_GPIO, 0);
            ESP_LOGI(TAG, "LED OFF");
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
