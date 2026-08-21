#include <stdint.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "driver/gpio.h"
#include "driver/ledc.h" //PWM制御のためのヘッダ
#include "hal/ledc_types.h"
#include "soc/clk_tree_defs.h"

void set_led_brightness(uint32_t duty);

static const char *TAG = "ESP7";

void app_main(void) {
    ledc_timer_config_t ledc_timer = {
        .speed_mode     = LEDC_LOW_SPEED_MODE,  //速度モード
        .timer_num      = LEDC_TIMER_0,         //タイマー番号
        .duty_resolution = LEDC_TIMER_13_BIT,   //解像度 (明るさを表現。 0 ~ 8191 = 2^13 <-bit数)
        .freq_hz        = 5000,                 //周波数(何の?)
        .clk_cfg        = LEDC_AUTO_CLK         //クロックソースとは?
    };

    ledc_channel_config_t ledc_channel = {
        .speed_mode = LEDC_LOW_SPEED_MODE,//速度モードとは?
        .channel    = LEDC_CHANNEL_0,    //?
        .timer_sel  = LEDC_TIMER_0,      //ledc_timerで設定したタイマー番号
        .intr_type  = LEDC_INTR_DISABLE, //割り込み禁止?
        .gpio_num   = 2, //GPIO番号(LED)
        .duty       = 0, //デューティ比
        .hpoint     = 0  //
    };

    //PWM出力の準備
    //タイマー設定
    ledc_timer_config(&ledc_timer);
    //チャンネル設定
    ledc_channel_config(&ledc_channel);

    while (1) {
        ESP_LOGI(TAG, "消灯 Duty:0");
        set_led_brightness(2000);
        vTaskDelay(pdMS_TO_TICKS(500));

        ESP_LOGI(TAG, "点灯(暗) Duty:2000");
        set_led_brightness(2000);
        vTaskDelay(pdMS_TO_TICKS(500));

        ESP_LOGI(TAG, "点灯(明) Duty: 8000");
        set_led_brightness(8000);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void set_led_brightness(uint32_t duty) {
    //値を設定する
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
    //設定をLEDに反映する
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}
