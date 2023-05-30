#include "esp_log.h"
#include "main_usb.h"

// Interface method switcher
#define IFM_I2C             0           // I2C
#define IFM_SPI             1           // SPI
#define IFM_UART            2           // UART
#define IF_METHOD           IFM_UART

#if IF_METHOD == IFM_I2C
    #include "main_i2c.h"
#elif IF_METHOD == IFM_SPI
    #include "main_spi.h"
#elif IF_METHOD == IFM_UART
    #include "main_uart.h"
#endif

#define TASK_STACK_SIZE     (4 * 1024)
#define TASK_PRIORITY       17
#define MAIN_WAIT_TICK      100

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
    usb_start();
    ESP_LOGI(TAG, "Initialize finished");

    TaskHandle_t task;
#if IF_METHOD == IFM_I2C
    xTaskCreate(i2c_task_receive_data, "receive_data", TASK_STACK_SIZE, NULL, TASK_PRIORITY, &task);
#elif IF_METHOD == IFM_SPI
    xTaskCreate(spi_task_receive_data, "receive_data", TASK_STACK_SIZE, NULL, TASK_PRIORITY, &task);
#elif IF_METHOD == IFM_UART
    xTaskCreate(uart_task_receive_data, "receive_data", TASK_STACK_SIZE, NULL, TASK_PRIORITY, &task);
#endif
    ESP_ERROR_CHECK(task == NULL);
    ESP_LOGI(TAG, "Task created");

    while (true) {
        vTaskDelay(MAIN_WAIT_TICK);
    }
}