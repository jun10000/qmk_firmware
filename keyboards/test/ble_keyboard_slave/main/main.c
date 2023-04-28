#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

// Interface method
#define IFM_INPUT_I2C           1       // I2C
#define IFM_INPUT_SPI           2       // SPI
#define IFM_INPUT_UART          3       // UART
#define IFM_OUTPUT_USB          1       // USB
#define IFM_OUTPUT_BL           2       // Bluetooth LE

#define IFM_INPUT_USE           IFM_INPUT_UART
#define IFM_OUTPUT_USE          IFM_OUTPUT_BL
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

static const char *TAG = "ble-keyboard";

#if IFM_INPUT_USE == IFM_INPUT_I2C
    #include "main_i2c.h"
#elif IFM_INPUT_USE == IFM_INPUT_SPI
    #include "main_spi.h"
#elif IFM_INPUT_USE == IFM_INPUT_UART
    #include "main_uart.h"
#endif
#if IFM_OUTPUT_USE == IFM_OUTPUT_USB
    #include "main_usb.h"
#elif IFM_OUTPUT_USE == IFM_OUTPUT_BL
    #include "main_bl.h"
#endif



void app_main(void)
{

//--------------------------------------------------
// Initialize input interface
//--------------------------------------------------

#if IFM_INPUT_USE == IFM_INPUT_I2C
    i2c_start();
#elif IFM_INPUT_USE == IFM_INPUT_SPI
    spi_start();
#elif IFM_INPUT_USE == IFM_INPUT_UART
    uart_start();
#endif
    ESP_LOGI(TAG, "Initialize input interface finished");

//--------------------------------------------------
// Initialize output interface
//--------------------------------------------------

#if IFM_OUTPUT_USE == IFM_OUTPUT_USB
    usb_start();
#elif IFM_OUTPUT_USE == IFM_OUTPUT_BL
    bl_start();
#endif
    ESP_LOGI(TAG, "Initialize output interface finished");

//--------------------------------------------------
// Initialize task data
//--------------------------------------------------

    task_data_t task_data = {
        .queue = xQueueCreate(QUEUE_LENGTH, sizeof(queue_data_t)),
    };
    ESP_ERROR_CHECK(task_data.queue == 0);
    ESP_LOGI(TAG, "Initialize task data finished");

//--------------------------------------------------
// Create input task
//--------------------------------------------------

    TaskHandle_t task_input;
#if IFM_INPUT_USE == IFM_INPUT_I2C
    xTaskCreate(i2c_task_receive_data, "i2c_task_receive_data", TASK_STACK_SIZE, &task_data, TASK_PRIORITY_INPUT, &task_input);
#elif IFM_INPUT_USE == IFM_INPUT_SPI
    xTaskCreate(spi_task_receive_data, "spi_task_receive_data", TASK_STACK_SIZE, &task_data, TASK_PRIORITY_INPUT, &task_input);
#elif IFM_INPUT_USE == IFM_INPUT_UART
    xTaskCreate(uart_task_receive_data, "uart_task_receive_data", TASK_STACK_SIZE, &task_data, TASK_PRIORITY_INPUT, &task_input);
#endif
    ESP_ERROR_CHECK(task_input == NULL);
    ESP_LOGI(TAG, "Input task created");

//--------------------------------------------------
// Create output task
//--------------------------------------------------

    TaskHandle_t task_output;
#if IFM_OUTPUT_USE == IFM_OUTPUT_USB
    xTaskCreate(usb_task_transmit_data, "usb_task_transmit_data", TASK_STACK_SIZE, &task_data, TASK_PRIORITY_OUTPUT, &task_output);
#elif IFM_OUTPUT_USE == IFM_OUTPUT_BL
    xTaskCreate(bl_task_transmit_data, "bl_task_transmit_data", TASK_STACK_SIZE, &task_data, TASK_PRIORITY_OUTPUT, &task_output);
#endif
    ESP_ERROR_CHECK(task_output == NULL);
    ESP_LOGI(TAG, "Output task created");

//--------------------------------------------------
// Do nothing
//--------------------------------------------------

    while (true) {
        vTaskDelay(MAIN_WAIT_TICK);
    }

}
