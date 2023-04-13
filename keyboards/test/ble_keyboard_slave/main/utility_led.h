#pragma once

#include "led_strip.h"

#define LED_PIN         48
#define LED_RES_FREQ    (10 * 1000 * 1000)
#define LED_COLOR       255, 255, 255

static const char *LED_TAG = "ble-keyboard-led";

static led_strip_handle_t led_handle;

void led_start(void) {
    led_strip_config_t conf_strip = {
        .strip_gpio_num = LED_PIN,
        .max_leds = 1,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB,
        .led_model = LED_MODEL_SK6812,
        .flags.invert_out = 0,
    };

    led_strip_rmt_config_t conf_rmt = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = LED_RES_FREQ,
        .mem_block_symbols = 0,
        .flags.with_dma = 0,
    };

    ESP_ERROR_CHECK(led_strip_new_rmt_device(&conf_strip, &conf_rmt, &led_handle));
    ESP_LOGI(LED_TAG, "Initialize LED finished");
}

void led_enable(void) {
    if (!led_handle) {
        led_start();
    }

    ESP_ERROR_CHECK(led_strip_set_pixel(led_handle, 0, LED_COLOR));
    ESP_ERROR_CHECK(led_strip_refresh(led_handle));
}

void led_disable(void) {
    if (!led_handle) {
        led_start();
    }

    ESP_ERROR_CHECK(led_strip_clear(led_handle));
}
