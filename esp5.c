#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"

#define BUTTON_GPIO 0
#define LED_GPIO 2

static const char *TAG = "ESP5";

void app_main(void) {
    //GPIOピン初期化
    gpio_reset_pin(BUTTON_GPIO);
    gpio_reset_pin(LED_GPIO);

    //GPIOピン目的設定
    gpio_set_direction(BUTTON_GPIO, GPIO_MODE_INPUT);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

    //入力ノイズ低減のためのプルアップ抵抗ON
    gpio_set_pull_mode(BUTTON_GPIO, GPIO_PULLUP_ONLY);

    ESP_LOGI(TAG, "Your esp32 is waiting your input...");

    int led_state = 0;
    int current_button_state = 1; //ボタンは押すと1のため
    int last_button_state = 1;

    while(1) {
        //ボタン入力取得
        current_button_state = gpio_get_level(BUTTON_GPIO);

        //押された瞬間を判定
        if (current_button_state == 0 && last_button_state == 1) {
            led_state = !led_state;
        }

        gpio_set_level(LED_GPIO, led_state);
        vTaskDelay(pdMS_TO_TICKS(50));

        last_button_state = current_button_state;
    }
}
