#pragma once

int bl_gatt_access_cb(uint16_t conn_handle, uint16_t attr_handle,
                      struct ble_gatt_access_ctxt *ctxt, void *arg) {
    bool is_descriptor = (ctxt->op == BLE_GATT_ACCESS_OP_READ_DSC ||
                          ctxt->op == BLE_GATT_ACCESS_OP_WRITE_DSC);
    bool is_read = (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR ||
                    ctxt->op == BLE_GATT_ACCESS_OP_READ_DSC);
    uint16_t uuid = ble_uuid_u16(is_descriptor ? ctxt->dsc->uuid : ctxt->chr->uuid);
    int index = (int)arg;

    ESP_LOGI(BL_TAG, "%s %s access occured, uuid = %04X, index = 0x%x, attr_handle = 0x%x",
        is_descriptor ? "Descriptor" : "Characteristic",
        is_read ? "read" : "write",
        uuid, index, attr_handle);

    switch (index) {
        case BL_INDEX_CHR_BATTERY_LEVEL:
            uint8_t battery_level = 80;
            return nimble_mbuf_write(ctxt->om, &battery_level, 1);
        case BL_INDEX_CHR_MANUFACTURER_NAME:
            char *manufacturer_name = USB_STRING_MANUFACTURER;
            return nimble_mbuf_write(ctxt->om, manufacturer_name, strlen(manufacturer_name));
        case BL_INDEX_CHR_MODEL_NUMBER:
            char *model_number = USB_STRING_PRODUCT;
            return nimble_mbuf_write(ctxt->om, model_number, strlen(model_number));
        case BL_INDEX_CHR_FIRMWARE_REVISION:
            char *firmware_revision = "0x0100";
            return nimble_mbuf_write(ctxt->om, firmware_revision, strlen(firmware_revision));
        case BL_INDEX_CHR_SOFTWARE_REVISION:
            char *software_revision = "0x0100";
            return nimble_mbuf_write(ctxt->om, software_revision, strlen(software_revision));
        case BL_INDEX_CHR_REPORT_KEYBOARD_INPUT:
            if (uxQueueMessagesWaiting(bl_queue_keyboard) == 0) {
                return 0;
            }

            hid_keyboard_report_t report_keyboard_input;
            if (xQueueReceive(bl_queue_keyboard, &report_keyboard_input, 0) != pdTRUE) {
                ESP_LOGE(BL_TAG, "Receive keyboard report from the queue failed");
                return 0;
            }

            ESP_LOGI(BL_TAG,
                "Sending keyboard report\n"
                "    modifier = 0x%x\n"
                "    keycode = {0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x}",
                report_keyboard_input.modifier,
                report_keyboard_input.keycode[0],
                report_keyboard_input.keycode[1],
                report_keyboard_input.keycode[2],
                report_keyboard_input.keycode[3],
                report_keyboard_input.keycode[4],
                report_keyboard_input.keycode[5]);
            return nimble_mbuf_write(ctxt->om, &report_keyboard_input, sizeof(report_keyboard_input));
        case BL_INDEX_CHR_REPORT_MOUSE_INPUT:
            return 0;
        case BL_INDEX_CHR_REPORT_KEYBOARD_OUTPUT:
            uint8_t report_keyboard_output;
            int report_keyboard_output_result = nimble_mbuf_read(ctxt->om, &report_keyboard_output, 1);
            if (report_keyboard_output_result != 0) {
                return report_keyboard_output_result;
            }

            if (report_keyboard_output & KEYBOARD_LED_CAPSLOCK) {
                led_enable();
            } else {
                led_disable();
            }

            ESP_LOGI(BL_TAG, "Received keyboard output report, led = %02X", report_keyboard_output);
            return 0;
        case BL_INDEX_CHR_REPORT_MAP:
            return nimble_mbuf_write(ctxt->om, USB_REPORT_DESCRIPTOR, sizeof(USB_REPORT_DESCRIPTOR));
        case BL_INDEX_CHR_HID_INFORMATION:
            bl_chr_hid_information_t hid_information = {
                .bcdHID = BL_VALUE_USB_BCDHID,
                .bCountryCode = BL_VALUE_HIDI_CC_NOT_LOCALIZED,
                .Flags = BL_F_CHR_HIDI_REMOTE_WAKE,
            };

            return nimble_mbuf_write(ctxt->om, &hid_information, sizeof(hid_information));
        case BL_INDEX_CHR_HID_CONTROL_POINT:
            uint8_t hid_control_point;
            int hid_control_point_result = nimble_mbuf_read(ctxt->om, &hid_control_point, 1);
            if (hid_control_point_result != 0) {
                return hid_control_point_result;
            }

            xQueueReset(queue_input_if);
            xQueueReset(bl_queue_keyboard);
            memset(&bl_report_keyboard, 0, sizeof(bl_report_keyboard));
            bl_is_suspended = !hid_control_point;

            ESP_LOGI(BL_TAG, "HID Host is %s", hid_control_point ? "resumed" : "suspended");
            return 0;
        case BL_INDEX_DSC_CPF_BATTERY_LEVEL:
            bl_dsc_cpf_t cpf_battery_level = {
                .format = BL_VALUE_CPF_FORMAT_UINT8,
                .exponent = 0,
                .unit = BL_UUID_UNIT_PERCENTAGE,
                .name_space = BL_VALUE_CPF_NAME_SPACE_BTSIG,
                .description = BL_VALUE_CPF_DESCRIPTION_UNKNOWN,
            };

            return nimble_mbuf_write(ctxt->om, &cpf_battery_level, sizeof(cpf_battery_level));
        case BL_INDEX_DSC_RR_REPORT_KEYBOARD_INPUT:
            bl_dsc_rr_t rr_report_keyboard_input = {
                .report_id = HID_ITF_PROTOCOL_KEYBOARD,
                .report_type = BL_VALUE_RR_REPORT_TYPE_INPUT,
            };
            return nimble_mbuf_write(ctxt->om, &rr_report_keyboard_input, sizeof(rr_report_keyboard_input));
        case BL_INDEX_DSC_RR_REPORT_MOUSE_INPUT:
            bl_dsc_rr_t rr_report_mouse_input = {
                .report_id = HID_ITF_PROTOCOL_MOUSE,
                .report_type = BL_VALUE_RR_REPORT_TYPE_INPUT,
            };
            return nimble_mbuf_write(ctxt->om, &rr_report_mouse_input, sizeof(rr_report_mouse_input));
        case BL_INDEX_DSC_RR_REPORT_KEYBOARD_OUTPUT:
            bl_dsc_rr_t rr_report_keyboard_output = {
                .report_id = HID_ITF_PROTOCOL_KEYBOARD,
                .report_type = BL_VALUE_RR_REPORT_TYPE_OUTPUT,
            };
            return nimble_mbuf_write(ctxt->om, &rr_report_keyboard_output, sizeof(rr_report_keyboard_output));
        default:
            return BLE_ATT_ERR_UNLIKELY;
    }

    return 0;
}

//
// Characteristics
//

static const struct ble_gatt_chr_def BL_GATT_CHR_BATTERY_LEVEL = {
    .uuid = BLE_UUID16_DECLARE(BL_UUID_CHR_BATTERY_LEVEL),
    .access_cb = bl_gatt_access_cb,
    .arg = (void *)BL_INDEX_CHR_BATTERY_LEVEL,
    .descriptors = (struct ble_gatt_dsc_def[]){
        {
            .uuid = BLE_UUID16_DECLARE(BL_UUID_DSC_CPF),
            .att_flags = BL_F_DSC_READ,
            .min_key_size = 0,
            .access_cb = bl_gatt_access_cb,
            .arg = (void *)BL_INDEX_DSC_CPF_BATTERY_LEVEL,
        },
        {
            0,
        },
    },
    .flags = BL_F_CHR_READ | BLE_GATT_CHR_F_NOTIFY,
    .min_key_size = 0,
    .val_handle = &bl_val_handle_list[BL_INDEX_CHR_BATTERY_LEVEL],
};

static const struct ble_gatt_chr_def BL_GATT_CHR_MANUFACTURER_NAME = {
    .uuid = BLE_UUID16_DECLARE(BL_UUID_CHR_MANUFACTURER_NAME),
    .access_cb = bl_gatt_access_cb,
    .arg = (void *)BL_INDEX_CHR_MANUFACTURER_NAME,
    .descriptors = NULL,
    .flags = BL_F_CHR_READ,
    .min_key_size = 0,
    .val_handle = &bl_val_handle_list[BL_INDEX_CHR_MANUFACTURER_NAME],
};

static const struct ble_gatt_chr_def BL_GATT_CHR_MODEL_NUMBER = {
    .uuid = BLE_UUID16_DECLARE(BL_UUID_CHR_MODEL_NUMBER),
    .access_cb = bl_gatt_access_cb,
    .arg = (void *)BL_INDEX_CHR_MODEL_NUMBER,
    .descriptors = NULL,
    .flags = BL_F_CHR_READ,
    .min_key_size = 0,
    .val_handle = &bl_val_handle_list[BL_INDEX_CHR_MODEL_NUMBER],
};

static const struct ble_gatt_chr_def BL_GATT_CHR_FIRMWARE_REVISION = {
    .uuid = BLE_UUID16_DECLARE(BL_UUID_CHR_FIRMWARE_REVISION),
    .access_cb = bl_gatt_access_cb,
    .arg = (void *)BL_INDEX_CHR_FIRMWARE_REVISION,
    .descriptors = NULL,
    .flags = BL_F_CHR_READ,
    .min_key_size = 0,
    .val_handle = &bl_val_handle_list[BL_INDEX_CHR_FIRMWARE_REVISION],
};

static const struct ble_gatt_chr_def BL_GATT_CHR_SOFTWARE_REVISION = {
    .uuid = BLE_UUID16_DECLARE(BL_UUID_CHR_SOFTWARE_REVISION),
    .access_cb = bl_gatt_access_cb,
    .arg = (void *)BL_INDEX_CHR_SOFTWARE_REVISION,
    .descriptors = NULL,
    .flags = BL_F_CHR_READ,
    .min_key_size = 0,
    .val_handle = &bl_val_handle_list[BL_INDEX_CHR_SOFTWARE_REVISION],
};

static const struct ble_gatt_chr_def BL_GATT_CHR_REPORT_KEYBOARD_INPUT = {
    .uuid = BLE_UUID16_DECLARE(BL_UUID_CHR_REPORT),
    .access_cb = bl_gatt_access_cb,
    .arg = (void *)BL_INDEX_CHR_REPORT_KEYBOARD_INPUT,
    .descriptors = (struct ble_gatt_dsc_def[]){
        {
            .uuid = BLE_UUID16_DECLARE(BL_UUID_DSC_RR),
            .att_flags = BL_F_DSC_READ,
            .min_key_size = 0,
            .access_cb = bl_gatt_access_cb,
            .arg = (void *)BL_INDEX_DSC_RR_REPORT_KEYBOARD_INPUT,
        },
        {
            0,
        },
    },
    .flags = BL_F_CHR_READ | BLE_GATT_CHR_F_NOTIFY,
    .min_key_size = 0,
    .val_handle = &bl_val_handle_list[BL_INDEX_CHR_REPORT_KEYBOARD_INPUT],
};

static const struct ble_gatt_chr_def BL_GATT_CHR_REPORT_MOUSE_INPUT = {
    .uuid = BLE_UUID16_DECLARE(BL_UUID_CHR_REPORT),
    .access_cb = bl_gatt_access_cb,
    .arg = (void *)BL_INDEX_CHR_REPORT_MOUSE_INPUT,
    .descriptors = (struct ble_gatt_dsc_def[]){
        {
            .uuid = BLE_UUID16_DECLARE(BL_UUID_DSC_RR),
            .att_flags = BL_F_DSC_READ,
            .min_key_size = 0,
            .access_cb = bl_gatt_access_cb,
            .arg = (void *)BL_INDEX_DSC_RR_REPORT_MOUSE_INPUT,
        },
        {
            0,
        },
    },
    .flags = BL_F_CHR_READ | BLE_GATT_CHR_F_NOTIFY,
    .min_key_size = 0,
    .val_handle = &bl_val_handle_list[BL_INDEX_CHR_REPORT_MOUSE_INPUT],
};

static const struct ble_gatt_chr_def BL_GATT_CHR_REPORT_KEYBOARD_OUTPUT = {
    .uuid = BLE_UUID16_DECLARE(BL_UUID_CHR_REPORT),
    .access_cb = bl_gatt_access_cb,
    .arg = (void *)BL_INDEX_CHR_REPORT_KEYBOARD_OUTPUT,
    .descriptors = (struct ble_gatt_dsc_def[]){
        {
            .uuid = BLE_UUID16_DECLARE(BL_UUID_DSC_RR),
            .att_flags = BL_F_DSC_READ,
            .min_key_size = 0,
            .access_cb = bl_gatt_access_cb,
            .arg = (void *)BL_INDEX_DSC_RR_REPORT_KEYBOARD_OUTPUT,
        },
        {
            0,
        },
    },
    .flags = BL_F_CHR_READ | BL_F_CHR_WRITE | BLE_GATT_CHR_F_WRITE_NO_RSP,
    .min_key_size = 0,
    .val_handle = &bl_val_handle_list[BL_INDEX_CHR_REPORT_KEYBOARD_OUTPUT],
};

static const struct ble_gatt_chr_def BL_GATT_CHR_REPORT_MAP = {
    .uuid = BLE_UUID16_DECLARE(BL_UUID_CHR_REPORT_MAP),
    .access_cb = bl_gatt_access_cb,
    .arg = (void *)BL_INDEX_CHR_REPORT_MAP,
    .descriptors = NULL,
    .flags = BL_F_CHR_READ,
    .min_key_size = 0,
    .val_handle = &bl_val_handle_list[BL_INDEX_CHR_REPORT_MAP],
};

static const struct ble_gatt_chr_def BL_GATT_CHR_HID_INFORMATION = {
    .uuid = BLE_UUID16_DECLARE(BL_UUID_CHR_HID_INFORMATION),
    .access_cb = bl_gatt_access_cb,
    .arg = (void *)BL_INDEX_CHR_HID_INFORMATION,
    .descriptors = NULL,
    .flags = BL_F_CHR_READ,
    .min_key_size = 0,
    .val_handle = &bl_val_handle_list[BL_INDEX_CHR_HID_INFORMATION],
};

static const struct ble_gatt_chr_def BL_GATT_CHR_HID_CONTROL_POINT = {
    .uuid = BLE_UUID16_DECLARE(BL_UUID_CHR_HID_CONTROL_POINT),
    .access_cb = bl_gatt_access_cb,
    .arg = (void *)BL_INDEX_CHR_HID_CONTROL_POINT,
    .descriptors = NULL,
    .flags = BLE_GATT_CHR_F_WRITE_NO_RSP,
    .min_key_size = 0,
    .val_handle = &bl_val_handle_list[BL_INDEX_CHR_HID_CONTROL_POINT],
};

//
// Characteristic list
//

static const struct ble_gatt_chr_def BL_GATT_BAS_CHARACTERISTICS[] = {
    BL_GATT_CHR_BATTERY_LEVEL,
    {0},
};

static const struct ble_gatt_chr_def BL_GATT_DIS_CHARACTERISTICS[] = {
    BL_GATT_CHR_MANUFACTURER_NAME,
    BL_GATT_CHR_MODEL_NUMBER,
    BL_GATT_CHR_FIRMWARE_REVISION,
    BL_GATT_CHR_SOFTWARE_REVISION,
    {0},
};

static const struct ble_gatt_chr_def BL_GATT_HID_CHARACTERISTICS[] = {
    BL_GATT_CHR_REPORT_KEYBOARD_INPUT,
    BL_GATT_CHR_REPORT_MOUSE_INPUT,
    BL_GATT_CHR_REPORT_KEYBOARD_OUTPUT,
    BL_GATT_CHR_REPORT_MAP,
    BL_GATT_CHR_HID_INFORMATION,
    BL_GATT_CHR_HID_CONTROL_POINT,
    {0},
};

//
// Services
//

static const struct ble_gatt_svc_def BL_GATT_SERVICE_BAS = {
    .type = BLE_GATT_SVC_TYPE_PRIMARY,
    .uuid = BLE_UUID16_DECLARE(BL_UUID_SERVICE_BAS),
    .includes = NULL,
    .characteristics = BL_GATT_BAS_CHARACTERISTICS,
};

static const struct ble_gatt_svc_def BL_GATT_SERVICE_DIS = {
    .type = BLE_GATT_SVC_TYPE_PRIMARY,
    .uuid = BLE_UUID16_DECLARE(BL_UUID_SERVICE_DIS),
    .includes = NULL,
    .characteristics = BL_GATT_DIS_CHARACTERISTICS,
};

static const struct ble_gatt_svc_def BL_GATT_SERVICE_HID = {
    .type = BLE_GATT_SVC_TYPE_PRIMARY,
    .uuid = BLE_UUID16_DECLARE(BL_UUID_SERVICE_HID),
    .includes = NULL,
    .characteristics = BL_GATT_HID_CHARACTERISTICS,
};

static const struct ble_gatt_svc_def BL_GATT_SERVICES[] = {
    BL_GATT_SERVICE_BAS,
    BL_GATT_SERVICE_DIS,
    BL_GATT_SERVICE_HID,
    {0},
};
