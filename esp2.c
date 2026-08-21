#include <stdio.h>
#include "freertos/FreeRTOS.h" //free RT "OS"?
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "hal/gpio_types.h"

#define LED_GPIO1 1
#define LED_GPIO2 2
#define LED_GPIO3 3

static const char *TAG = "ESP2";

void app_main(void)
{
    gpio_reset_pin(LED_GPIO1);
    gpio_reset_pin(LED_GPIO2);
    gpio_reset_pin(LED_GPIO3);
    gpio_set_direction(LED_GPIO1, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_GPIO2, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_GPIO3, GPIO_MODE_OUTPUT);

    int count = 0;

    while(1) {
        if (count % 2 == 0) {
            gpio_set_level(LED_GPIO1, 1);
            gpio_set_level(LED_GPIO2, 0);
            gpio_set_level(LED_GPIO3, 0);
            ESP_LOGI(TAG, "time:%d", count);
        }
        if (count % 3 == 0) {
            gpio_set_level(LED_GPIO1, 0);
            gpio_set_level(LED_GPIO2, 1);
            gpio_set_level(LED_GPIO3, 0);
        }
        else {
            gpio_set_level(LED_GPIO1, 0);
            gpio_set_level(LED_GPIO2, 0);
            gpio_set_level(LED_GPIO3, 1);
            ESP_LOGI(TAG, "time:%d", count);
        }
        count++;
        vTaskDelay(pdMS_TO_TICKS(333));


    }

}

/*
 * gpio_set_
 * -> level    :毎回変更。電圧変動
 *    direction:モード設定。今回は1回だけで良い
 */
