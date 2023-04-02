#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

// Interface method switcher
#define IFM_I2C             0           // I2C (Not updated)
#define IFM_SPI             1           // SPI (Not updated)
#define IFM_UART            2           // UART
#define IF_METHOD           IFM_UART

#define TASK_STACK_SIZE         (4 * 1024)
#define TASK_PRIORITY_INPUT     17
#define TASK_PRIORITY_OUTPUT    16
#define QUEUE_LENGTH            10
#define MAIN_WAIT_TICK          100

typedef struct {
    uint8_t keycode_high;
    uint8_t keycode_low;
    bool key_pressed;
} queue_data_t;

typedef struct {
    QueueHandle_t queue;
} task_data_t;

#if IF_METHOD == IFM_I2C
    #include "main_i2c.h"
#elif IF_METHOD == IFM_SPI
    #include "main_spi.h"
#elif IF_METHOD == IFM_UART
    #include "main_uart.h"
#endif
#include "main_usb.h"

static const char *TAG = "ble-keyboard";



void app_main(void)
{
#if IF_METHOD == IFM_I2C
    i2c_start();
#elif IF_METHOD == IFM_SPI
    spi_start();
#elif IF_METHOD == IFM_UART
    uart_start();
#endif
    ESP_LOGI(TAG, "Initialize input interface finished");


    usb_start();
    ESP_LOGI(TAG, "Initialize output interface finished");


    task_data_t task_data = {
        .queue = xQueueCreate(QUEUE_LENGTH, sizeof(queue_data_t)),
    };
    ESP_ERROR_CHECK(task_data.queue == 0);
    ESP_LOGI(TAG, "Initialize task data finished");


    TaskHandle_t task_input;
#if IF_METHOD == IFM_I2C
    xTaskCreate(i2c_task_receive_data, "i2c_task_receive_data", TASK_STACK_SIZE, &task_data, TASK_PRIORITY_INPUT, &task_input);
#elif IF_METHOD == IFM_SPI
    xTaskCreate(spi_task_receive_data, "spi_task_receive_data", TASK_STACK_SIZE, &task_data, TASK_PRIORITY_INPUT, &task_input);
#elif IF_METHOD == IFM_UART
    xTaskCreate(uart_task_receive_data, "uart_task_receive_data", TASK_STACK_SIZE, &task_data, TASK_PRIORITY_INPUT, &task_input);
#endif
    ESP_ERROR_CHECK(task_input == NULL);
    ESP_LOGI(TAG, "Input task created");


    TaskHandle_t task_output;
    xTaskCreate(usb_task_transmit_data, "usb_task_transmit_data", TASK_STACK_SIZE, &task_data, TASK_PRIORITY_OUTPUT, &task_output);
    ESP_ERROR_CHECK(task_output == NULL);
    ESP_LOGI(TAG, "Output task created");


    while (true) {
        vTaskDelay(MAIN_WAIT_TICK);
    }
}