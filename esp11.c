//GPIO 34を使用して電圧を測定

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h" //単発のアナログ読み取りのため
#include "hal/adc_types.h"
//Analog to Digital Converter

static const char *TAG = "ESP11";

void app_main(void) {
    //adcユニット初期化
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    adc_oneshot_new_unit(&init_config1, &adc1_handle);

    //チャンネルの設定
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,   //アッテネーター(減衰器)
    };
    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_6, &config);

    ESP_LOGI(TAG, "Input Voltage...");

    while(1) {
        int raw_value = 0; //ループ内で毎回使用する変数は宣言し直す

        //値の読み取り、電圧への変換
        adc_oneshot_read(adc1_handle, ADC_CHANNEL_6, &raw_value);
        float voltage = ((float)raw_value / 4095.0f) * 3.3f;

        ESP_LOGI(TAG, "Raw: %4d | Voltage: %.2f V", raw_value, voltage);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
