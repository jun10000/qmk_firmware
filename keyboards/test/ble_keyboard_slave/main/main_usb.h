#pragma once

#include "esp_log.h"
#include "tinyusb.h"

#define USB_MAX_CURRENT         100
#define USB_MAX_PACKET_LEN      16
#define USB_POLLING_MS          10

// Each index and value of STRING_LIST elements
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

static const char * STRING_LIST[] = {
    USB_STRING_LANGUAGE,
    USB_STRING_MANUFACTURER,
    USB_STRING_PRODUCT,
    USB_STRING_SERIAL,
    USB_STRING_HID,
};

static const uint8_t REPORT_DESCRIPTOR_LIST[] = {
    TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(HID_ITF_PROTOCOL_KEYBOARD)),
    TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(HID_ITF_PROTOCOL_MOUSE)),
};

static const uint8_t CONFIGURATION_DESCRIPTOR_ETC_LIST[] = {
    TUD_CONFIG_DESCRIPTOR(1, 1, USB_STRING_INDEX_LANGUAGE, TUD_CONFIG_DESC_LEN + CFG_TUD_HID * TUD_HID_DESC_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, USB_MAX_CURRENT),
    TUD_HID_DESCRIPTOR(0, USB_STRING_INDEX_HID, false, sizeof(REPORT_DESCRIPTOR_LIST), 0x81, USB_MAX_PACKET_LEN, USB_POLLING_MS),       // 0x81 = Number 1, IN Pipe
};



void usb_start(void) {
    const tinyusb_config_t conf = {
        .device_descriptor = NULL,
        .string_descriptor = STRING_LIST,
        .string_descriptor_count = sizeof(STRING_LIST) / sizeof(STRING_LIST[0]),
        .external_phy = false,
        .configuration_descriptor = CONFIGURATION_DESCRIPTOR_ETC_LIST,
        .self_powered = false,
        .vbus_monitor_io = 0,
    };

    ESP_ERROR_CHECK(tinyusb_driver_install(&conf));
}
