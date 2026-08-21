#include <stdint.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "hal/ledc_types.h"
#include "soc/clk_tree_defs.h"

void set_led_brightness(uint32_t duty);

static const char *TAG = "ESP8";

void app_main(void) {
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = LEDC_TIMER_13_BIT,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK
    };

    ledc_channel_config_t ledc_channel = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = 2,
        .duty = 0,
        .hpoint = 0 //位相
    };

    ledc_timer_config(&ledc_timer);
    ledc_channel_config(&ledc_channel);

    ESP_LOGI(TAG, "Brighting..");

    while(1) {
        for (int duty = 0; duty <= 8191; duty += 50) {
            set_led_brightness((uint32_t)duty);
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        for (int duty = 8192; duty >= 0; duty -= 50) {
            set_led_brightness((uint32_t)duty);
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
}

void set_led_brightness(uint32_t duty) {
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}
