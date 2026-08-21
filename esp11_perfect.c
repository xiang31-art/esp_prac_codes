// ====================================================================
// ESP32 ADC 電圧測定プログラム（第11問 完成版）
// 概要: GPIO 34 を使用してアナログ電圧を入力し、デジタル値およびV単位に変換して出力する。
// ====================================================================

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h" // ADC操作用の最新ライブラリ

static const char *TAG = "ESP11";

void app_main(void) {
    // ----------------------------------------------------------------
    // 1. ADCユニット（ハードウェア本体）の初期化
    // ----------------------------------------------------------------
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1, // ADC1を使用
    };
    adc_oneshot_new_unit(&init_config1, &adc1_handle);

    // ----------------------------------------------------------------
    // 2. ADCチャンネル（個別のピン設定）の設定
    // ----------------------------------------------------------------
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT, // 解像度: 12bit (0〜4095)
        .atten = ADC_ATTEN_DB_12,         // アッテネータ: 0〜3.3V測定対応
    };
    // GPIO 34 は ADC_CHANNEL_6 に割り当てられています
    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_6, &config);

    ESP_LOGI(TAG, "ADC Voltmeter Started. Connect GPIO 34 to target voltage.");

    // ----------------------------------------------------------------
    // 3. メイン計測ループ
    // ----------------------------------------------------------------
    while(1) {
        // 毎回ループ内で読み取り用変数を0に初期化
        int raw_sum = 0;

        // 10回読み込んで平均をとる（ノイズ対策）
        for (int i = 0; i < NO_OF_SAMPLES; i++) {
                    int raw = 0;
                    // GPIO 34 (ADC_CHANNEL_6) から生のデジタル値を取得
                    adc_oneshot_read(adc1_handle, ADC_CHANNEL_6, &raw);
                    raw_sum += raw;
                    vTaskDelay(pdMS_TO_TICKS(10)); // 短いディレイ
                }

        // 平均Raw値の計算
        int raw_avg = raw_sum / NO_OF_SAMPLES;

        // 生の値 (0〜4095) を実際の電圧値 (0.0V〜3.3V) に換算
        float voltage = ((float)raw_avg / 4095.0f) * 3.3f;

        // 測定結果をログに出力
        ESP_LOGI(TAG, "Raw: %4d | Voltage: %.2f V", raw_avg, voltage);

        // 1秒待機
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
