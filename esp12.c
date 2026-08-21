#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "hal/adc_types.h"

static const char *TAG = "ESP12";

void app_main(void) {
    //ADCユニット作成・初期化
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_conf1 = {
        .unit_id = ADC_UNIT_1,
    };
    adc_oneshot_new_unit(&init_conf1, &adc1_handle);

    //チャンネル設定
    adc_oneshot_chan_cfg_t channel_conf = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12
    };
    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_6, &channel_conf);

    printf("Raw, Voltage\n");

    while(1) {
        int raw_value = 0;
        adc_oneshot_read(adc1_handle, ADC_CHANNEL_6, &raw_value);

        float voltage = ((float)raw_value / 4095.0f) * 3.3f;

        printf("%d, %.3f\n", raw_value, voltage);
        fflush(stdout);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
