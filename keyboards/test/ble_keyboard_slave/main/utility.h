#pragma once

uint8_t get_checksum(uint8_t *bytes, int length) {
    uint8_t sum = 0x00;
    for (int i = 0; i < length; i++) {
        sum += bytes[i];
    }

    return sum;
}

void print_input_data(const char *TAG_NAME, queue_data_t *data) {
    ESP_LOGI(TAG_NAME, "Data received");
    ESP_LOGI(TAG_NAME, "    High byte of keycode: 0x%02X", data->keycode_high);
    ESP_LOGI(TAG_NAME, "    Low byte of keycode: 0x%02X", data->keycode_low);
    ESP_LOGI(TAG_NAME, "    Key pressed: %s", data->key_pressed ? "yes" : "no");
}

void array_uint8_add(uint8_t *array, uint8_t array_len, uint8_t item) {
    for (uint8_t i = 0; i < array_len; i++) {
        if (array[i] == 0) {
            array[i] = item;
            return;
        }
    }
}

void array_uint8_remove(uint8_t *array, uint8_t array_len, uint8_t item) {
    for (uint8_t i = 0; i < array_len; i++) {
        if (array[i] == item) {
            array[i] = 0;
            return;
        }
    }
}
