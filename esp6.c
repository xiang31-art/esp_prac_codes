#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "driver/gpio.h"

#define BUTTON_GPIO 0
#define LED_GPIO 2

static void gpio_isr_handler(void* arg);

static const char *TAG = "ESP6";

static volatile bool button_pressed = false;

void app_main(void) {
    gpio_reset_pin(BUTTON_GPIO);
    gpio_reset_pin(LED_GPIO);

    gpio_set_direction(BUTTON_GPIO, GPIO_MODE_INPUT);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

    gpio_set_pull_mode(BUTTON_GPIO, GPIO_PULLUP_ONLY);

    //割り込みトリガー設定
    gpio_set_intr_type(BUTTON_GPIO, GPIO_INTR_NEGEDGE);

    //割り込みサービスのインストール・有効化
    gpio_install_isr_service(0);

    //GPIOピンと関数結びつけ
    gpio_isr_handler_add(BUTTON_GPIO, gpio_isr_handler, NULL);

    while(1) {
        if (button_pressed) {
            button_pressed = false;
            gpio_set_level(LED_GPIO, 1);
            ESP_LOGI(TAG, "LED ON!! (Triggered by Interrupt)");
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

static void IRAM_ATTR gpio_isr_handler(void* arg) {
    button_pressed = true;
    //gpio_set_level(LED_GPIO, 1);
    //ESP_LOGI(TAG, "LED ON!!");
    //重い処理は割り込みハンドラ内で呼び出してはいけない
}
