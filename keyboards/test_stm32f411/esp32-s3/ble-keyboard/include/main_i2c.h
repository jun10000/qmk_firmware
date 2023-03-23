#pragma once

#include "esp_log.h"
#include "driver/i2c.h"
#include "utility.h"

#define I2C_PORT            I2C_NUM_0
#define I2C_PIN_SCL         0
#define I2C_PIN_SDA         1
#define I2C_ADDRESS         0x55
#define I2C_CLOCK_SPEED     100000
#define I2C_WAIT_TICK       10          // Bothersome...
#define I2C_BUF_LENGTH      128
#define I2C_TRANS_LENGTH    28          // Bothersome...
#define I2C_DATA_LENGTH     5

static const char *I2C_TAG = "ble-keyboard-i2c";

void i2c_start(void) {
    i2c_config_t conf = {
        .mode = I2C_MODE_SLAVE,
        .sda_io_num = I2C_PIN_SDA,
        .scl_io_num = I2C_PIN_SCL,
        .sda_pullup_en = false,
        .scl_pullup_en = false,
        .slave.addr_10bit_en = 0,
        .slave.slave_addr = I2C_ADDRESS,
        .slave.maximum_speed = I2C_CLOCK_SPEED,
        .clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL,
    };

    ESP_ERROR_CHECK(i2c_param_config(I2C_PORT, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_PORT, conf.mode, I2C_BUF_LENGTH, 0, 0));
}

void i2c_task_receive_data(void *param) {
    uint8_t data[I2C_BUF_LENGTH];

    while (true) {
        int len = i2c_slave_read_buffer(I2C_PORT, data, I2C_BUF_LENGTH, I2C_WAIT_TICK);
        ESP_ERROR_CHECK(len < 0);

        if (len == 0) {
            continue;
        }

        for (int i = 0; i < len - I2C_DATA_LENGTH; i++) {
            if (data[i] != 0xFF) {
                continue;
            } else if (get_checksum(&data[i + 1], I2C_DATA_LENGTH - 2) != data[i + I2C_DATA_LENGTH - 1]) {
                ESP_LOGE(I2C_TAG, "Received data are wrong");
                continue;
            }

            ESP_LOGI(I2C_TAG, "Data received [%d - %d]", i, i + I2C_DATA_LENGTH - 1);
            ESP_LOGI(I2C_TAG, "    High byte of keycode: 0x%02X", data[i + 1]);
            ESP_LOGI(I2C_TAG, "    Low byte of keycode: 0x%02X", data[i + 2]);
            ESP_LOGI(I2C_TAG, "    Key pressed: %s", data[i + 3] ? "yes" : "no");
            i += I2C_TRANS_LENGTH - 1;
        }
    }
}
