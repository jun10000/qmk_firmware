#pragma once

#include <string.h>
#include "esp_log.h"
#include "driver/spi_slave.h"
#include "utility.h"

#define SPI_PORT            SPI2_HOST
#define SPI_PIN_CS          10
#define SPI_PIN_SCLK        12
#define SPI_PIN_MOSI        11
#define SPI_PIN_MISO        13
#define SPI_MODE            0           // CPOL * 2 + CPHA
#define SPI_QUEUE_SIZE      3
#define SPI_DATA_LENGTH     4

static const char *SPI_TAG = "ble-keyboard-spi";

void spi_start(void) {
    spi_bus_config_t conf_bus = {
        .mosi_io_num = SPI_PIN_MOSI,
        .miso_io_num = SPI_PIN_MISO,
        .sclk_io_num = SPI_PIN_SCLK,
        .data2_io_num = -1,
        .data3_io_num = -1,
        .data4_io_num = -1,
        .data5_io_num = -1,
        .data6_io_num = -1,
        .data7_io_num = -1,
        .max_transfer_sz = 0,
        .flags = 0,
        .intr_flags = 0,
    };

    spi_slave_interface_config_t conf_slave = {
        .spics_io_num = SPI_PIN_CS,
        .flags = 0,
        .queue_size = SPI_QUEUE_SIZE,
        .mode = SPI_MODE,
        .post_setup_cb = NULL,
        .post_trans_cb = NULL,
    };

    ESP_ERROR_CHECK(spi_slave_initialize(SPI_PORT, &conf_bus, &conf_slave, SPI_DMA_DISABLED));
}

void spi_task_receive_data(void *param) {
    task_data_t *task_data = param;
    uint8_t data[SPI_DATA_LENGTH];
    spi_slave_transaction_t *trans_receive;

    const spi_slave_transaction_t trans_send = {
        .length = SPI_DATA_LENGTH * 8,
        .trans_len = 0,
        .tx_buffer = NULL,
        .rx_buffer = data,
        .user = NULL,
    };

    while (true) {
        memset(data, 0, SPI_DATA_LENGTH);
        trans_receive = NULL;

        ESP_ERROR_CHECK(spi_slave_queue_trans(SPI_PORT, &trans_send, portMAX_DELAY));
        ESP_ERROR_CHECK(spi_slave_get_trans_result(SPI_PORT, &trans_receive, portMAX_DELAY));

        if (get_checksum(&data[0], SPI_DATA_LENGTH - 1) != data[SPI_DATA_LENGTH - 1]) {
            ESP_LOGE(SPI_TAG, "Received data are wrong");
            continue;
        }

        queue_data_t qdata = {
            .keycode_high = data[0],
            .keycode_low = data[1],
            .key_pressed = data[2],
        };

        print_input_data(SPI_TAG, &qdata);
        if (xQueueSend(task_data->queue, &qdata, 0) != pdTRUE) {
            ESP_LOGE(SPI_TAG, "Send data to the queue failed");
        }
    }
}
