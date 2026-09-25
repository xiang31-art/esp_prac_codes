//サーミスタ温度計測テスト　by INA219

#include "driver/i2c_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "driver/i2c_master.h"
#include "hal/i2c_types.h"
#include "soc/clk_tree_defs.h"
#include "soc/gpio_num.h"
#include <stdint.h>
#include <math.h>

const char* TAG = "ESP15";
const float B_CONST = 3420.5;
const float VCC = 5.144;
const float ABSOLUTE_ZERO = 273.15;

//各種値保存用
typedef struct {
    int temp_l; //測定温度範囲の上端
    int temp_h; //測定温度範囲の下端
    double r_h; //上端温度のときの抵抗値
    double r_m; //中間の温度の抵抗値
    double r_l; //上端温度のときの抵抗値
    double r_connection; //接続する抵抗の抵抗値
    double b_const; //B定数
} setting;

double derive_resistance_from_voltage(double v_x, double v_cc, double r_connection);
double derive_temp_from_resistance(int temp_l, double b_const, double r_l, double r_th);

void app_main(void) {
    ESP_LOGI(TAG, "Hello1!");

    /*i2cバス初期設定*/
    i2c_master_bus_config_t bus_conf = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = -1,
        .scl_io_num = GPIO_NUM_22,
        .sda_io_num = GPIO_NUM_21,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    ESP_LOGI(TAG, "Hello2!");



    //ハンドル作成・エラーチェック
    i2c_master_bus_handle_t bus_handle_p;
    i2c_master_bus_handle_t bus_handle_th;

    //ソーラーパネル用
    esp_err_t ret_init_bus_handle_p = i2c_new_master_bus(&bus_conf, &bus_handle_p);
    //サーミスタ用
    esp_err_t ret_init_bus_handle_th = i2c_new_master_bus(&bus_conf, &bus_handle_th);

    if (ret_init_bus_handle_p != ESP_OK || ret_init_bus_handle_th != ESP_OK) {
        ESP_LOGE(TAG, "I2C master bus_p initialize failed: %s , %s", esp_err_to_name(ret_init_bus_handle_p), esp_err_to_name(ret_init_bus_handle_th));
        return;
    }

    ESP_LOGI(TAG, "Hello3!");

    //デバイス設定
    i2c_device_config_t dev_conf_p = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,  //length of data
        .device_address = 0X40,
        .scl_speed_hz = 100000
    };

    i2c_device_config_t dev_conf_th = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,  //length of data
        .device_address = 0X41,
        .scl_speed_hz = 100000
    };


    i2c_master_dev_handle_t dev_handle_p;
    i2c_master_dev_handle_t dev_handle_th;

    ESP_LOGI(TAG, "Hello4!");


    //ハンドルに操作兼付与
    esp_err_t ret_dev_p = i2c_master_bus_add_device(bus_handle_p, &dev_conf_p, &dev_handle_p);
    esp_err_t ret_dev_th = i2c_master_bus_add_device(bus_handle_th, &dev_conf_th, &dev_handle_th);
    if (ret_dev_p != ESP_OK || ret_dev_th != ESP_OK) {
        ESP_LOGE(TAG, "I2C device_p add failed: %s , %s", esp_err_to_name(ret_dev_p), esp_err_to_name(ret_dev_th));
        return;
    }

    ESP_LOGI(TAG, "Hello5!");



    uint8_t measure_config_data[3] = {0x00, 0x39, 0x9F};
    const uint8_t register_shuntV = 0x01;
    const uint8_t register_busV = 0x02;

    ESP_LOGI(TAG, "Hello6!");

    //測定の設定情報書き込み
    esp_err_t ret_config_p = i2c_master_transmit(dev_handle_p, measure_config_data, sizeof(measure_config_data), -1);
    ESP_LOGI(TAG, "Hello7!");
    esp_err_t ret_config_th = i2c_master_transmit(dev_handle_th, measure_config_data, sizeof(measure_config_data), -1);

    if (ret_config_p != ESP_OK || ret_config_th != ESP_OK) {
        ESP_LOGE(TAG, "INA219 configure failed: %s", esp_err_to_name(ret_config_p));
        return;
    }

    ESP_LOGI(TAG, "Hello8!");

    //サーミスタの情報
    setting sett = {
        .temp_l = 20,
        .temp_h = 80,
        .r_l = 12110, //抵抗値はデータシートより
        .r_m = 4147,  //単位はΩ
        .r_h = 1668,
        .r_connection = 3000,
        .b_const = 3420.5
    };

    ESP_LOGI(TAG, "Hello9");

    //ほとんどesp14_resistorと同じ
    printf("Voltage[V],Current[mA],Power[W],Resistance[Ω],Vth[V],Rt[Ω],Temp[℃]\n");
    while(1) {
        uint8_t shuntV_data_p[2];
        uint8_t busV_data_p[2];
        esp_err_t ret_shuntV_p = i2c_master_transmit_receive(dev_handle_p, &register_shuntV, 1, shuntV_data_p, 2, -1);
        esp_err_t ret_busV_p = i2c_master_transmit_receive(dev_handle_p, &register_busV, 1, busV_data_p, 2, -1);

        //読み込みエラーチェック
        if (ret_shuntV_p != ESP_OK || ret_busV_p != ESP_OK) {
            ESP_LOGE(TAG, "I2C read failed shuntV:%s busV:%s", esp_err_to_name(ret_shuntV_p), esp_err_to_name(ret_busV_p));
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        /*データ変換 (電圧)*/
        //1.2バイト分連結
        uint16_t busV_raw_conbine_p = (busV_data_p[0] << 8) | busV_data_p[1];
        //2.関係のない下位3bit切\り捨て
        uint16_t busV_raw_p =  busV_raw_conbine_p >> 3;
        //3.mVからVへの変換
        float voltage_p = busV_raw_p * 0.004f;

        /*データ変換 (電流)*/
        //1.電圧と同様、ビットシフト
        int16_t shuntV_raw_conbine_p = (int16_t)((shuntV_data_p[0] << 8) | shuntV_data_p[1]);
        //2.シャント電圧のバイナリを数値に変換
        float shunt_voltage_mV_p = shuntV_raw_conbine_p * 0.01f;    //INA219の1bitは0.01mVらしい
        //3.shuntV_raw_conbineはシャント抵抗の電圧降下のため、シャント抵抗の抵抗値とともに、オームの法則で電流を導出
        float current_mA_p = shunt_voltage_mV_p / 0.1f;  //10を掛けても等しい

        //電力の計算
        float power_mW_p = voltage_p * current_mA_p;
        float power_W_p = power_mW_p / 1000;    //mWからWへの変換

        //抵抗値の計算
        float resistance_p = -1;
        if (current_mA_p != 0) {
            resistance_p = (voltage_p / current_mA_p) * 1000;
        }
        /*ここまでソーラーパネル*/


        /*サーミスタ*/
        //1.電圧測定
        uint8_t busV_data_th[2];
        esp_err_t ret_busV_th = i2c_master_transmit_receive(dev_handle_th, &register_busV, 1, busV_data_th, 2, -1);
        if (ret_busV_th != ESP_OK) {
            ESP_LOGE(TAG, "I2C read failed Thermistor busV:%s", esp_err_to_name(ret_busV_th));
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }


        //電圧の変換
        uint16_t busV_raw_conbine_th = (busV_data_th[0] << 8) | busV_data_th[1];
        uint16_t busV_raw_th = busV_raw_conbine_th >> 3;
        float voltage_th = busV_raw_th * 0.004;

        //2.電圧を抵抗値に変換
        float resistance_th =derive_resistance_from_voltage(voltage_th, VCC, sett.r_connection);

        //3.抵抗値を温度に変換
        float current_temp = derive_temp_from_resistance(sett.temp_l, sett.b_const, sett.r_l, resistance_th);
        /*ここまでサーミスタ*/



        //表示
        if (resistance_p != -1) { //抵抗値が計算されたとき
            printf("%.3f,%.1f,%.3f,%.3f\n", voltage_p, current_mA_p, power_W_p, resistance_p); //パネル
            printf("%.3f,%.3f,%.3f\n", voltage_th, resistance_th, current_temp);    //サーミスタ
        }
        else {
            printf("%.3f,%.1f,%.3f,-,", voltage_p, current_mA_p, power_W_p); //パネル
            printf("%.3f,%.3f,%.3f\n", voltage_th, resistance_th, current_temp); //サーミスタ
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


//電圧から抵抗値
double derive_resistance_from_voltage(double v_x, double v_cc, double r_connection) {
    double r_t = v_x / (v_cc - v_x) * r_connection;
    return r_t;
}


//抵抗値から温度
double derive_temp_from_resistance(int temp_l, double b_const, double r_l, double r_th) {
    //絶対温度への変換
    double t0 = temp_l + ABSOLUTE_ZERO;

    double t1 = b_const / (log(r_th / r_l) + b_const * (1.0 / t0));
    return t1 - ABSOLUTE_ZERO;  //セルシウス度に戻す
}
