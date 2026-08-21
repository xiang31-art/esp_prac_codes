/*
 * 1:include
 * 2:app name
 * 3:prototype
 * 4:app_main
 * 5:gpio_reset
 * 6:gpio set direction
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "hal/gpio_types.h"

typedef struct {
    int gpio_num;
    int delay_time;
} led_config;

void blink_led_pattern(const led_config *config);

static const char *TAG = "ESP3";

void app_main(void) {
    led_config test;
    test.gpio_num = 2;
    test.delay_time = 50;

    gpio_reset_pin(test.gpio_num);
    gpio_set_direction(test.gpio_num, GPIO_MODE_OUTPUT);

    ESP_LOGI(TAG, "LED Blinking...");

    while(1) {
        blink_led_pattern(&test);
    }
}

//LEDの点滅
//blink: ライトが点滅する
void blink_led_pattern(const led_config *config) {
    gpio_set_level(config->gpio_num, 1);
    vTaskDelay(pdMS_TO_TICKS(config->delay_time));

    gpio_set_level(config->gpio_num, 0);
    vTaskDelay(pdMS_TO_TICKS(config->delay_time));
    //Periodic Delay (周期的な遅延)
}
