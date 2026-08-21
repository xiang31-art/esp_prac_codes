#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "MY APP"; //アプリの名前的な？どんな役割?

void app_main(void)
{
    int count = 0;

    while(1) {
        ESP_LOGI(TAG, "Hello ESP32! Count:%d",count);
        //種類がある
        // ESP_LOGI :log information?
        // ESP_LOGE :log error　など
        count++;
        vTaskDelay(pdMS_TO_TICKS(3000));
        //vTaskDelayとは？
        // pdMS? MS:milli sec. pd?
    }
}
