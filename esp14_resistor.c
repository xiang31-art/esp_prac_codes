//リード線・はんだ線抵抗測定用

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "driver/i2c_types.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "hal/i2c_types.h"
#include "soc/clk_tree_defs.h"

const char *TAG = "ESP14";

void app_main(void) {
    //i2cバス初期設定
    i2c_master_bus_config_t bus_conf = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = -1,
        .scl_io_num = GPIO_NUM_22,//クロック線
        .sda_io_num = GPIO_NUM_21,//データ線
        .glitch_ignore_cnt = 7,//7:基準値
        .flags.enable_internal_pullup = true
    };
    i2c_master_bus_handle_t bus_handle;
    //ハンドル作成・エラーチェック
    esp_err_t ret_init = i2c_new_master_bus(&bus_conf, &bus_handle);
    if (ret_init != ESP_OK) {
        ESP_LOGE(TAG, "I2C master bus initialize failed: %s", esp_err_to_name(ret_init));
        return;
    }


    i2c_device_config_t dev_conf = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,//データのbit数
        .device_address = 0X40, //INA219のデータアドレス
        .scl_speed_hz = 100000  //通信速度100kHz
    };

    i2c_master_dev_handle_t dev_handle;

    //ハンドルに操作権を付与
    esp_err_t ret_dev = i2c_master_bus_add_device(bus_handle,&dev_conf, &dev_handle);
    if (ret_dev != ESP_OK) {
        ESP_LOGE(TAG, "I2C device add failed: %s", esp_err_to_name(ret_dev));
        return;
    }

    uint8_t measure_config_data[3] = {0x00, 0x39, 0x9F};
    const uint8_t register_shuntV = 0x01;
    const uint8_t register_busV = 0x02;

    //測定の設定情報書き込み
    esp_err_t ret_config = i2c_master_transmit(dev_handle, measure_config_data, sizeof(measure_config_data), -1);
    if (ret_config != ESP_OK) {
        ESP_LOGE(TAG, "INA219 configure failed: %s", esp_err_to_name(ret_config));
        return;
    }

    printf("Voltage[V],Current[mA],Power[mW]\n");
    while(1) {
        uint8_t shuntV_data[2];
        uint8_t busV_data[2];
        esp_err_t ret_shuntV = i2c_master_transmit_receive(dev_handle, &register_shuntV, 1, shuntV_data, 2, -1);
        esp_err_t ret_busV = i2c_master_transmit_receive(dev_handle, &register_busV, 1, busV_data, 2, -1);

        //読み込みエラーチェック
        if (ret_shuntV != ESP_OK || ret_busV != ESP_OK) {
            ESP_LOGE(TAG, "I2C read failed shuntV:%s busV:%s", esp_err_to_name(ret_shuntV), esp_err_to_name(ret_busV));
            vTaskDelay(pdMS_TO_TICKS(1000));
            //return; //止まるため
            continue;
        }

        /*データ変換 (電圧)*/
        //1.2バイト分連結
        uint16_t busV_raw_conbine = (busV_data[0] << 8) | busV_data[1];
        //2.関係のない下位3bit切\り捨て
        uint16_t busV_raw =  busV_raw_conbine >> 3;
        //3.mVからVへの変換
        float voltage = busV_raw * 0.004f;

        /*データ変換 (電流)*/
        //1.電圧と同様、ビットシフト
        int16_t shuntV_raw_conbine = (int16_t)((shuntV_data[0] << 8) | shuntV_data[1]);
        //2.シャント電圧のバイナリを数値に変換
        float shunt_voltage_mV = shuntV_raw_conbine * 0.01f;    //INA219の1bitは0.01mVらしい
        //3.shuntV_raw_conbineはシャント抵抗の電圧降下のため、シャント抵抗の抵抗値とともに、オームの法則で電流を導出
        float current_mA = shunt_voltage_mV / 0.1f;  //10を掛けても等しい

        //電力の計算
        float power_mW = voltage * current_mA;

        //抵抗値の計算
        if (current_mA != 0) {
            float resistance = (voltage / current_mA) * 1000;
            printf("%.3f V  %.3f mA  %.3f mW  %.3f　Ω\n", voltage, current_mA, power_mW, resistance);
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }


        printf("%.3f V  %.3f mA  %.3f mW  -　Ω\n", voltage, current_mA, power_mW);

        //csv用
        //printf("%.3f,%.1f,%.3f\n", voltage, current_mA, power_mW / 1000);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}


/*データ変換
 * ・取得した値のビットシフトは電流に対しては行わない->行う。0x01に入っているのも電圧のバイナリ。
 */
