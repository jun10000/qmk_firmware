#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"

#define DATA_LENGTH         5
#define TASK_STACK_SIZE     (4 * 1024)
#define MAIN_WAITTICK       100
#define IF_METHOD           IFM_I2C

#if IF_METHOD == IFM_I2C
    #define I2C_PORT        I2C_NUM_0
    #define I2C_SDA_NUM     4
    #define I2C_SCL_NUM     5
    #define I2C_ADDR        0x55
    #define I2C_BUF_LEN     128
    #define I2C_WAITTICK    200
    #define I2C_PRIORITY    17
#endif

static const char *TAG = "ble-keyboard";

uint8_t Checksum(uint8_t *bytes, int length) {
    uint8_t sum = 0x00;
    for (int i = 0; i < length; i++) {
        sum += bytes[i];
    }
    return sum;
}

void task_receiveData(void *param) {
    uint8_t data[I2C_BUF_LEN];
    int length;
    int i;

    while (true) {
        length = i2c_slave_read_buffer(I2C_PORT, data, I2C_BUF_LEN, I2C_WAITTICK);
        ESP_ERROR_CHECK(length < 0);
        if (length == 0) {
            continue;
        }

        for (i = 0; i < length - DATA_LENGTH; i++) {
            if (data[i] != 0xFF) {
                continue;
            }

            if (Checksum(&data[i + 1], DATA_LENGTH - 2) != data[i + DATA_LENGTH - 1]) {
                continue;
            }

            ESP_LOGI(TAG, "Data received [%d - %d]", i, i + DATA_LENGTH - 1);
            ESP_LOGI(TAG, "    High byte of keycode: 0x%02X", data[i + 1]);
            ESP_LOGI(TAG, "    Low byte of keycode: 0x%02X", data[i + 2]);
            ESP_LOGI(TAG, "    Key pressed: %s", data[i + 3] ? "yes" : "no");
        }

        i2c_reset_rx_fifo(I2C_PORT);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void app_main(void)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_SLAVE,
        .sda_io_num = I2C_SDA_NUM,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_SCL_NUM,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .slave.addr_10bit_en = 0,
        .slave.slave_addr = I2C_ADDR,
    };

    ESP_ERROR_CHECK(i2c_param_config(I2C_PORT, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_PORT, conf.mode, I2C_BUF_LEN, 0, 0));
    ESP_LOGI(TAG, "Initialize finished");

    TaskHandle_t task;
    xTaskCreate(task_receiveData, "receiveData", TASK_STACK_SIZE, NULL, I2C_PRIORITY, &task);
    ESP_ERROR_CHECK(task == NULL);
    ESP_LOGI(TAG, "Task created");

    while (true) {
        vTaskDelay(MAIN_WAITTICK);
    }

    vTaskDelete(task);
    ESP_ERROR_CHECK(i2c_driver_delete(I2C_PORT));
    ESP_LOGI(TAG, "Finalize finished");
}