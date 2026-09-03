#include <stdint.h>
#include <stdio.h>
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "soc/clk_tree_defs.h"
#include "soc/gpio_num.h"

static const char *TAG = "ESP13";

void app_main(void) {
    //I2Cバスの初期設定?
    i2c_master_bus_config_t i2c_master_conf = {
        .clk_source = I2C_CLK_SRC_DEFAULT,  //クロックソース
        .i2c_port = -1, //esp32のi2cポート設定。-1で自動割当
        .scl_io_num = GPIO_NUM_22,  //SCL(クロック線):Serial Clock Line
        .sda_io_num = GPIO_NUM_21,  //SDA(データ線)　 :Seriarl Data line
        .glitch_ignore_cnt = 7, //ノイズ対策のフィルタ(基準値7)
        //構造体の中の構造体
        .flags.enable_internal_pullup = true //内部プルアップ抵抗設定
    };

    //1:ハンドルの作成
    i2c_master_bus_handle_t bus_handle;

    //I2Cバスの初期化とエラーチェック(ESP32のi2c通信を有効化)
    esp_err_t ret_init = i2c_new_master_bus(&i2c_master_conf , &bus_handle);//bus_handleに操作権がつく

    if (ret_init != ESP_OK) {
        //エラー内容の出力と処理の停止
        ESP_LOGE(TAG, "I2C master bus init failed: %s", esp_err_to_name(ret_init));
        return; //忘れない
    }
    ESP_LOGI(TAG, "I2C master bus initialized successflly!");


    //2:I2Cデバイスの追加
    //INA219の設定
    i2c_device_config_t dev_conf = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,  //INA219は7ビット
        .device_address = 0x40, //INA219のI2Cアドレス
        .scl_speed_hz = 100000  //通信速度。100kHzは標準的
    };

    //ハンドル作成
    i2c_master_dev_handle_t dev_handle;

    //ハンドルに操作権付与
    esp_err_t ret_dev = i2c_master_bus_add_device(bus_handle, &dev_conf, &dev_handle);

    //エラーチェック
    if (ret_dev != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add INA219 device: %s", esp_err_to_name(ret_dev));
        return;
    }
    ESP_LOGI(TAG, "INA219 device added successfully!");

/*
 * 0x399F ->
 */
    uint8_t config_data[3] = {0x00, 0x39, 0x9F};
    const uint8_t write = 0x02;
    //const uint8_t write_init = 0x00;

    esp_err_t ret_config = i2c_master_transmit(dev_handle, config_data, sizeof(config_data), -1);

    if (ret_config != ESP_OK) {
        ESP_LOGE(TAG, "INA219 config write failed: %s", esp_err_to_name(ret_config));
    }
    ESP_LOGI(TAG, "INA219 configured successfully!");

    while(1) {
        uint8_t data[2];    //毎回初期化
        esp_err_t ret_read =  i2c_master_transmit_receive(dev_handle, &write, 1, data, 2, -1);
        /* 引数
         * 1:デバイスのハンドル
        * 2:読み出しのレジスタ番号の配列
        * 3:送信データサイズ
        * 4:受信データ受け取り用の2バイト配列
        * 5:受信したいデータサイズ(INA219は2バイト)
        * 6:通信のタイムアウト時間(ミリ秒。-1でデフォルト)
        */

        //エラーチェック
        if (ret_read != ESP_OK) {
            ESP_LOGE(TAG, "Data receive failed:%s", esp_err_to_name(ret_read));
            return;
        }

        //データ変換処理
        //1:2バイトを連結して16bitに
        uint16_t raw_val = (data[0] << 8) | data[1];
        //順番:data[0]->data[1]なので、16bitに結合するときも　data[0]の8bit + data[1]の8bitの順

        //2:フラグである下位3bit切り捨て
        uint16_t bus_voltage_raw = raw_val >> 3;

        //3:mVからVへの変換 (1カウント = 4mV);
        float voltage = bus_voltage_raw * 0.004f;


        printf("Raw: 0x%04X, Shifted: %d, Voltage: %.3f V\n", raw_val, bus_voltage_raw, voltage);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}

/*
 * I2C通信のためのフロー
 * 1:I2Cバスの初期化
 * ・i2c_new_master_bus()
 * ・i2c_master_bus_config_t
 * ・i2c_master_bus_handle_t
 *
 * 2:I2Cデバイスの初期化・追加
 * ・i2c_master_bus_add_device()
 * ・i2c_device_config_t
 * ・i2c_master_dev_handle_t
 *
 * 3:データ送信・受信
 * ・
 *
 *
 * 4:データの計算と表示
 * 　printf()
 */
