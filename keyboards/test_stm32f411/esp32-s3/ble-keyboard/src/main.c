#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "driver/spi_slave.h"

// Interface method switcher
#define IFM_I2C             0           // I2C
#define IFM_SPI             1           // SPI
#define IF_METHOD           IFM_SPI

#define DATA_LENGTH         5
#define TASK_STACK_SIZE     (4 * 1024)
#define TASK_PRIORITY       17
#define MAIN_WAITTICK       100

#if IF_METHOD == IFM_I2C
    #define I2C_PORT        I2C_NUM_0
    #define I2C_PIN_SDA     4
    #define I2C_PIN_SCL     5
    #define I2C_ADDRESS     0x55
    #define I2C_BUF_LENGTH  128
    #define I2C_WAITTICK    200
#elif IF_METHOD == IFM_SPI
    #define SPI_PORT        SPI2_HOST
    #define SPI_PIN_CS      10
    #define SPI_PIN_SCLK    12
    #define SPI_PIN_MOSI    11
    #define SPI_PIN_MISO    13
    #define SPI_MODE        0           // CPOL * 2 + CPHA
    #define SPI_QUEUE_SIZE  3
#endif

static const char *TAG = "ble-keyboard";

uint8_t get_checksum(uint8_t *bytes, int length) {
    uint8_t sum = 0x00;
    for (int i = 0; i < length; i++) {
        sum += bytes[i];
    }
    return sum;
}



#if IF_METHOD == IFM_I2C
void i2c_start(void) {
    i2c_config_t conf = {
        .mode = I2C_MODE_SLAVE,
        .sda_io_num = I2C_PIN_SDA,
        .scl_io_num = I2C_PIN_SCL,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .slave.addr_10bit_en = 0,
        .slave.slave_addr = I2C_ADDRESS,
        .slave.maximum_speed = 0,
        .clk_flags = 0,
    };

    ESP_ERROR_CHECK(i2c_param_config(I2C_PORT, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_PORT, conf.mode, I2C_BUF_LENGTH, 0, 0));
}
#endif

#if IF_METHOD == IFM_SPI
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

    ESP_ERROR_CHECK(spi_slave_initialize(SPI_PORT, &conf_bus, &conf_slave, SPI_DMA_CH_AUTO));
}
#endif



#if IF_METHOD == IFM_I2C
void i2c_task_receive_data(void *param) {
    uint8_t data[I2C_BUF_LENGTH];
    int length;
    int i;

    while (true) {
        length = i2c_slave_read_buffer(I2C_PORT, data, I2C_BUF_LENGTH, I2C_WAITTICK);
        ESP_ERROR_CHECK(length < 0);
        if (length == 0) {
            continue;
        }

        for (i = 0; i < length - DATA_LENGTH; i++) {
            if (data[i] != 0xFF) {
                continue;
            }

            if (get_checksum(&data[i + 1], DATA_LENGTH - 2) != data[i + DATA_LENGTH - 1]) {
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
#endif

#if IF_METHOD == IFM_SPI
void spi_task_receive_data(void *param) {
    WORD_ALIGNED_ATTR uint8_t data[DATA_LENGTH];
    const spi_slave_transaction_t trans_send = {
        .length = DATA_LENGTH * 8,
        .trans_len = 0,
        .tx_buffer = NULL,
        .rx_buffer = data,
        .user = NULL,
    };

    while (true) {
        memset(data, 0, DATA_LENGTH);
        spi_slave_transaction_t *trans_receive;

        ESP_ERROR_CHECK(spi_slave_queue_trans(SPI_PORT, &trans_send, portMAX_DELAY));
        ESP_ERROR_CHECK(spi_slave_get_trans_result(SPI_PORT, &trans_receive, portMAX_DELAY));

        ESP_LOGI(TAG, "Data received");
        ESP_LOGI(TAG, "    High byte of keycode: 0x%02X", data[1]);
        ESP_LOGI(TAG, "    Low byte of keycode: 0x%02X", data[2]);
        ESP_LOGI(TAG, "    Key pressed: %s", data[3] ? "yes" : "no");
    }
}
#endif



void app_main(void)
{
#if IF_METHOD == IFM_I2C
    i2c_start();
#elif IF_METHOD == IFM_SPI
    spi_start();
#endif
    ESP_LOGI(TAG, "Initialize finished");

    TaskHandle_t task;
#if IF_METHOD == IFM_I2C
    xTaskCreate(i2c_task_receive_data, "receive_data", TASK_STACK_SIZE, NULL, TASK_PRIORITY, &task);
#elif IF_METHOD == IFM_SPI
    xTaskCreate(spi_task_receive_data, "receive_data", TASK_STACK_SIZE, NULL, TASK_PRIORITY, &task);
#endif
    ESP_ERROR_CHECK(task == NULL);
    ESP_LOGI(TAG, "Task created");

    while (true) {
        vTaskDelay(MAIN_WAITTICK);
    }
}