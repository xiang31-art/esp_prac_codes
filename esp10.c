#include <stdint.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "hal/gpio_types.h"
#include "hal/ledc_types.h"
#include "soc/clk_tree_defs.h"

#define LED_GPIO 2

void led_turn_on_duty(uint32_t duty);

static const char *TAG = "ESP10";

void app_main(void) {
    ledc_timer_config_t ledc_timer = {
        .timer_num = LEDC_TIMER_0,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_13_BIT,
        .clk_cfg = LEDC_AUTO_CLK,
        .freq_hz = 5000
    };

    ledc_channel_config_t ledc_channel = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = 2,
        .hpoint = 0,
        .duty = 0
    };

    gpio_reset_pin(LED_GPIO);
    //gpio_set_direction(LED_GPIO, GPIO_MODE_INPUT); これは必要ない(ledc_timer_configがあるため)

    ledc_timer_config(&ledc_timer);
    ledc_channel_config(&ledc_channel);

    ESP_LOGI(TAG, "Input 0~3");

    while(1) {
        int input = getchar();

        switch (input) {
            case '0' : {
                led_turn_on_duty(0);
                ESP_LOGI(TAG, "current duty: 0");
                break;
            }
            case '1' : {
                led_turn_on_duty(1000);
                ESP_LOGI(TAG, "current duty: 1000");
                break;
            }
            case '2' : {
                led_turn_on_duty(4000);
                ESP_LOGI(TAG, "current duty: 4000");
                break;
            }
            case '3' : {
                led_turn_on_duty(8191);
                ESP_LOGI(TAG, "current duty: 8192");
                break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void led_turn_on_duty(uint32_t duty) {
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}
