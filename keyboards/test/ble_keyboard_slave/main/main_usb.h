#pragma once

#define USB_MAX_CURRENT         100
#define USB_MAX_PACKET_LEN      16
#define USB_POLLING_MS          10
#define USB_LOOP_WAIT_MS        10

// Each index and value of USB_STRING_LIST elements
#define USB_STRING_INDEX_LANGUAGE       0
#define USB_STRING_LANGUAGE             (char[]){ 0x09, 0x04 }    // 0x0409 = English
#define USB_STRING_INDEX_MANUFACTURER   1
#define USB_STRING_MANUFACTURER         "Manufacturer0"
#define USB_STRING_INDEX_PRODUCT        2
#define USB_STRING_PRODUCT              "Product0"
#define USB_STRING_INDEX_SERIAL         3
#define USB_STRING_SERIAL               "Serial0"
#define USB_STRING_INDEX_HID            4
#define USB_STRING_HID                  "HID0"

static const char *USB_TAG = "ble-keyboard-usb";

static const char * USB_STRING_LIST[] = {
    USB_STRING_LANGUAGE,
    USB_STRING_MANUFACTURER,
    USB_STRING_PRODUCT,
    USB_STRING_SERIAL,
    USB_STRING_HID,
};

static const uint8_t USB_REPORT_DESCRIPTOR[] = {
    TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(HID_ITF_PROTOCOL_KEYBOARD)),
    TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(HID_ITF_PROTOCOL_MOUSE)),
};

static const uint8_t USB_CONFIGURATION_DESCRIPTOR_ETC[] = {
    TUD_CONFIG_DESCRIPTOR(1, 1, USB_STRING_INDEX_LANGUAGE, TUD_CONFIG_DESC_LEN + CFG_TUD_HID * TUD_HID_DESC_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, USB_MAX_CURRENT),
    TUD_HID_DESCRIPTOR(0, USB_STRING_INDEX_HID, false, sizeof(USB_REPORT_DESCRIPTOR), 0x81, USB_MAX_PACKET_LEN, USB_POLLING_MS),       // 0x81 = Number 1, IN Pipe
};



//
// User side functions
//

void usb_start(void) {
    const tinyusb_config_t conf = {
        .device_descriptor = NULL,
        .string_descriptor = USB_STRING_LIST,
        .string_descriptor_count = sizeof(USB_STRING_LIST) / sizeof(USB_STRING_LIST[0]),
        .external_phy = false,
        .configuration_descriptor = USB_CONFIGURATION_DESCRIPTOR_ETC,
        .self_powered = false,
        .vbus_monitor_io = 0,
    };

    ESP_ERROR_CHECK(tinyusb_driver_install(&conf));
}

void usb_task_transmit_data(void *param) {
    queue_data_t data;

    while (true) {
        if (!tud_mounted() || uxQueueMessagesWaiting(queue_input_if) == 0) {
            vTaskDelay(pdMS_TO_TICKS(USB_LOOP_WAIT_MS));
            continue;
        }

        if (xQueueReceive(queue_input_if, &data, 0) != pdTRUE) {
            ESP_LOGE(USB_TAG, "Receive data from the queue failed");
            continue;
        }

        if (data.key_pressed) {
            uint8_t send_keycodes[6] = { data.keycode_low };
            if (!tud_hid_keyboard_report(HID_ITF_PROTOCOL_KEYBOARD, 0, send_keycodes)) {
                ESP_LOGE(USB_TAG, "Send data to host failed");
            }
        } else {
            if (!tud_hid_keyboard_report(HID_ITF_PROTOCOL_KEYBOARD, 0, NULL)) {
                ESP_LOGE(USB_TAG, "Send data to host failed");
            }
        }
    }
}

void usb_receive_keyboard_data(uint8_t led_states) {
    if (led_states & KEYBOARD_LED_CAPSLOCK) {
        led_enable();
        ESP_LOGI(USB_TAG, "Capslock is enabled");
    } else {
        led_disable();
        ESP_LOGI(USB_TAG, "Capslock is disabled");
    }
}



//
// TinyUSB callback functions
//

uint8_t const * tud_hid_descriptor_report_cb(uint8_t instance) {
    return USB_REPORT_DESCRIPTOR;
}

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen) {
    return 0;
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) {
    switch (report_id) {
        case HID_ITF_PROTOCOL_KEYBOARD:
            if (report_type == HID_REPORT_TYPE_OUTPUT && bufsize == 1) {
                usb_receive_keyboard_data(buffer[0]);
            }
            break;
        case HID_ITF_PROTOCOL_MOUSE:
        default:
            break;
    }
}
