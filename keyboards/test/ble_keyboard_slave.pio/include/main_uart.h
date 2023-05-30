#pragma once

#include <string.h>
#include "esp_log.h"
#include "driver/uart.h"
#include "utility.h"

#define UART_PORT           UART_NUM_1
#define UART_PIN_TX         17
#define UART_PIN_RX         18
#define UART_BAUDRATE       115200
#define UART_DATA_LENGTH    4
#define UART_BUF_LENGTH     256
#define UART_QUEUE_SIZE     10
#define UART_READWAIT_TICK  100
#define UART_LOOPWAIT_MS    10

static const char *UART_TAG = "ble-keyboard-uart";

void uart_start(void) {
    uart_config_t conf = {
        .baud_rate = UART_BAUDRATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0,
        .source_clk = UART_SCLK_APB,
    };

    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &conf));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT, UART_PIN_TX, UART_PIN_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    QueueHandle_t handle;
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, UART_BUF_LENGTH, 0, UART_QUEUE_SIZE, &handle, 0));
}

void uart_task_receive_data(void *param) {
    int len;
    uint8_t data[UART_DATA_LENGTH];

    while (true) {
        len = 0;
        memset(data, 0, UART_DATA_LENGTH);

        ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_PORT, (size_t *)&len));
        len = uart_read_bytes(UART_PORT, data, len, UART_READWAIT_TICK);
        ESP_ERROR_CHECK(len == -1);

        if (len == 0) {
            vTaskDelay(pdMS_TO_TICKS(UART_LOOPWAIT_MS));
            continue;
        } else if (len != UART_DATA_LENGTH) {
            ESP_LOGE(UART_TAG, "Received data length is invalid");
            continue;
        } else if (get_checksum(&data[0], UART_DATA_LENGTH - 1) != data[UART_DATA_LENGTH - 1]) {
            ESP_LOGE(UART_TAG, "Received data are wrong");
            continue;
        }

        ESP_LOGI(UART_TAG, "Data received");
        ESP_LOGI(UART_TAG, "    High byte of keycode: 0x%02X", data[0]);
        ESP_LOGI(UART_TAG, "    Low byte of keycode: 0x%02X", data[1]);
        ESP_LOGI(UART_TAG, "    Key pressed: %s", data[2] ? "yes" : "no");
    }
}
