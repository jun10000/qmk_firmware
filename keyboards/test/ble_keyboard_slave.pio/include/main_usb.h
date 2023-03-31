#pragma once

#include "tinyusb.h"
#include "esp_log.h"

// #define UART_PORT           UART_NUM_1
// #define UART_PIN_TX         17
// #define UART_PIN_RX         18
// #define UART_BAUDRATE       115200
// #define UART_DATA_LENGTH    4
// #define UART_BUF_LENGTH     256
// #define UART_QUEUE_SIZE     10
// #define UART_READWAIT_TICK  100
// #define UART_LOOPWAIT_MS    10

static const char *USB_TAG = "ble-keyboard-usb";

void usb_start(void) {
    // ESP_ERROR_CHECK()
}

// void uart_task_receive_data(void *param) {
// }
