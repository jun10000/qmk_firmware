#pragma once

#define GPIO_PIN_BL_ADV         8
#define GPIO_PIN_INPUT_MASK     (1ULL << GPIO_PIN_BL_ADV)

// static const char *GPIO_TAG = "ble-keyboard-gpio";

void IRAM_ATTR gpio_isr_bl_adv(void *arg) {
    xQueueSendFromISR(queue_input_bl_adv, NULL, NULL);
}

void gpio_start(void) {
    gpio_config_t conf = {
        .pin_bit_mask = GPIO_PIN_INPUT_MASK,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = 1,
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_NEGEDGE,
    };

    ESP_ERROR_CHECK(gpio_config(&conf));
    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(gpio_isr_handler_add(GPIO_PIN_BL_ADV, gpio_isr_bl_adv, NULL));
}
