#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"

#define BUTTON_GPIO 0
#define LED_GPIO 2

static const char *TAG = "ESP4";

void app_main(void) {
    gpio_reset_pin(BUTTON_GPIO);
    gpio_reset_pin(LED_GPIO);

    gpio_set_direction(BUTTON_GPIO, GPIO_MODE_INPUT);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

    //入力ピンの電圧安定のために内部プルアップ抵抗を有効
    gpio_set_pull_mode(BUTTON_GPIO, GPIO_PULLUP_ONLY);

    ESP_LOGI(TAG, "your esp32 is waiting your input...");

    int button_status = 0;

    while(1) {
        button_status = gpio_get_level(BUTTON_GPIO);

        //ボタンが押された時
        if (button_status == 0) {
            gpio_set_level(LED_GPIO, 1);
            ESP_LOGI(TAG, "BUTTON Pushed!!!");
        }
        //ボタンが押されていない時
        else {
            gpio_set_level(LED_GPIO, 0);
        }
        //待機時間
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
