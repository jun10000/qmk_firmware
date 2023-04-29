#pragma once

extern void ble_store_config_init(void);

#define BL_F_CHR_HIDI_REMOTE_WAKE               1
#define BL_F_CHR_HIDI_NORMALLY_CONNECTABLE      2

#define BL_UUID_SERVICE_BAS                 0x180F      // Battery Service
#define BL_UUID_SERVICE_DIS                 0x180A      // Device Information Service
#define BL_UUID_SERVICE_HID                 0x1812      // HID Service
#define BL_UUID_CHR_BATTERY_LEVEL           0x2A19      // Battery Level
#define BL_UUID_CHR_MANUFACTURER_NAME       0x2A29      // Manufacturer Name String
#define BL_UUID_CHR_MODEL_NUMBER            0x2A24      // Model Number String
#define BL_UUID_CHR_SERIAL_NUMBER           0x2A25      // Serial Number String
#define BL_UUID_CHR_HARDWARE_REVISION       0x2A27      // Hardware Revision String
#define BL_UUID_CHR_FIRMWARE_REVISION       0x2A26      // Firmware Revision String
#define BL_UUID_CHR_SOFTWARE_REVISION       0x2A28      // Software Revision String
#define BL_UUID_CHR_SYSTEM_ID               0x2A23      // System ID
#define BL_UUID_CHR_PNP_ID                  0x2A50      // PnP ID
#define BL_UUID_CHR_REPORT                  0x2A4D      // Report
#define BL_UUID_CHR_REPORT_MAP              0x2A4B      // Report Map
#define BL_UUID_CHR_HID_INFORMATION         0x2A4A      // HID Information
#define BL_UUID_CHR_HID_CONTROL_POINT       0x2A4C      // HID Control Point
#define BL_UUID_DSC_CPF                     0x2904      // Characteristic Presentation Format
#define BL_UUID_DSC_RR                      0x2908      // Report Reference
#define BL_UUID_DSC_ERR                     0x2907      // External Report Reference
#define BL_UUID_UNIT_PERCENTAGE             0x27AD
#define BL_VALUE_APPEARANCE_KEYBOARD        0x03C1
#define BL_VALUE_USB_BCDHID                 0x0111      // USB HID 1.11
#define BL_VALUE_CPF_FORMAT_UINT8           0x04
#define BL_VALUE_CPF_NAME_SPACE_BTSIG       0x01
#define BL_VALUE_CPF_DESCRIPTION_UNKNOWN    0x0000
#define BL_VALUE_HIDI_CC_NOT_LOCALIZED      0x00
#define BL_VALUE_RR_REPORT_TYPE_INPUT       0x01
#define BL_VALUE_RR_REPORT_TYPE_OUTPUT      0x02
#define BL_VALUE_RR_REPORT_TYPE_FEATURE     0x03

#define BL_LOOP_WAIT_MS                     10

typedef enum {
    BL_INDEX_CHR_BATTERY_LEVEL = 0,
    BL_INDEX_CHR_MANUFACTURER_NAME,
    BL_INDEX_CHR_MODEL_NUMBER,
    BL_INDEX_CHR_SERIAL_NUMBER,
    BL_INDEX_CHR_HARDWARE_REVISION,
    BL_INDEX_CHR_FIRMWARE_REVISION,
    BL_INDEX_CHR_SOFTWARE_REVISION,
    BL_INDEX_CHR_SYSTEM_ID,
    BL_INDEX_CHR_PNP_ID,
    BL_INDEX_CHR_REPORT_KEYBOARD_INPUT,
    BL_INDEX_CHR_REPORT_MOUSE_INPUT,
    BL_INDEX_CHR_REPORT_KEYBOARD_OUTPUT,
    BL_INDEX_CHR_REPORT_FEATURE,
    BL_INDEX_CHR_REPORT_MAP,
    BL_INDEX_CHR_HID_INFORMATION,
    BL_INDEX_CHR_HID_CONTROL_POINT,
    BL_INDEX_CHR_MAX,                           // Characteristic count
    BL_INDEX_DSC_CPF_BATTERY_LEVEL,
    BL_INDEX_DSC_RR_REPORT_KEYBOARD_INPUT,
    BL_INDEX_DSC_RR_REPORT_MOUSE_INPUT,
    BL_INDEX_DSC_RR_REPORT_KEYBOARD_OUTPUT,
    BL_INDEX_DSC_RR_REPORT_FEATURE,
    BL_INDEX_DSC_ERR_REPORT_MAP,
} BL_INDEX_LIST;

typedef struct {
    uint16_t bcdHID;
    int8_t bCountryCode;
    uint8_t Flags;
} __attribute__((packed)) bl_chr_hid_information_t;

typedef struct {
    uint8_t format;
    int8_t exponent;
    uint16_t unit;
    uint8_t name_space;
    uint16_t description;
} __attribute__((packed)) bl_dsc_cpf_t;

typedef struct {
    uint8_t report_id;
    uint8_t report_type;
} __attribute__((packed)) bl_dsc_rr_t;

static const char *BL_TAG = "ble-keyboard-bl";

static uint8_t bl_address_type;
static uint16_t bl_val_handle_list[BL_INDEX_CHR_MAX];

static QueueHandle_t bl_queue_input;
static QueueHandle_t bl_queue_keyboard;
static hid_keyboard_report_t bl_report_keyboard;
static bool bl_is_connected;
static bool bl_is_suspended;
static uint16_t bl_conn_handle;

#include "main_bl_gatt.h"
#include "main_bl_gap.h"



//
// Callback functions
//

void ble_hs_cfg_gatts_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg) {
    char tmp[BLE_UUID_STR_LEN];

    switch (ctxt->op) {
        case BLE_GATT_REGISTER_OP_SVC:
            ESP_LOGI(BL_TAG, "Service %s (handle = %d) is registered",
                ble_uuid_to_str(ctxt->svc.svc_def->uuid, tmp),
                ctxt->svc.handle);
            break;
        case BLE_GATT_REGISTER_OP_CHR:
            ESP_LOGI(BL_TAG, "Characteristic %s (def_handle = %d, val_handle = %d) is registered",
                ble_uuid_to_str(ctxt->chr.chr_def->uuid, tmp),
                ctxt->chr.def_handle,
                ctxt->chr.val_handle);
            break;
        case BLE_GATT_REGISTER_OP_DSC:
            ESP_LOGI(BL_TAG, "Descriptor %s (handle = %d) is registered",
                ble_uuid_to_str(ctxt->dsc.dsc_def->uuid, tmp),
                ctxt->dsc.handle);
            break;
        default:
            ESP_ERROR_CHECK(ESP_FAIL);
            break;
    }
}

void ble_hs_cfg_reset_cb(int reason) {
    ESP_LOGE(BL_TAG, "Resetting state, reason = %d", reason);
}

void ble_hs_cfg_sync_cb(void) {
    ESP_ERROR_CHECK(ble_hs_util_ensure_addr(0));

    int result = ble_hs_id_infer_auto(0, &bl_address_type);
    if (result != 0) {
        ESP_LOGE(BL_TAG, "Determine address type failed, result = %d", result);
        return;
    }

    uint8_t address[6] = {0};
    result = ble_hs_id_copy_addr(bl_address_type, address, NULL);
    if (result != 0) {
        ESP_LOGE(BL_TAG, "Get address failed, result = %d", result);
        return;
    }

    ESP_LOGI(BL_TAG, "Device address = " MACSTR, MAC2STR(address));
    bl_gap_start_advertising();
}



//
// User side functions
//

esp_err_t bl_initialize_nvs_flash(void) {
    esp_err_t result = nvs_flash_init();
    if (result == ESP_ERR_NVS_NO_FREE_PAGES || result == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        result = nvs_flash_erase();
        if (result != ESP_OK) {
            return result;
        }

        result = nvs_flash_init();
    }

    return result;
}

void bl_initialize_ble_hs_cfg(void) {
    ble_hs_cfg.gatts_register_cb = ble_hs_cfg_gatts_register_cb;
    ble_hs_cfg.gatts_register_arg = NULL;
    ble_hs_cfg.sm_io_cap = BLE_SM_IO_CAP_NO_IO;
    ble_hs_cfg.sm_oob_data_flag = 0;
    ble_hs_cfg.sm_bonding = 1;
    ble_hs_cfg.sm_mitm = 1;
    ble_hs_cfg.sm_sc = 1;
    ble_hs_cfg.sm_keypress = 0;

    // to do: think later
    ble_hs_cfg.sm_our_key_dist = BLE_SM_PAIR_KEY_DIST_ENC;
    ble_hs_cfg.sm_their_key_dist = BLE_SM_PAIR_KEY_DIST_ENC;

    ble_hs_cfg.reset_cb = ble_hs_cfg_reset_cb;
    ble_hs_cfg.sync_cb = ble_hs_cfg_sync_cb;

    // "ble_store_config_init" will be called
    // ble_hs_cfg.store_read_cb = NULL;
    // ble_hs_cfg.store_write_cb = NULL;
    // ble_hs_cfg.store_delete_cb = NULL;

    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;
    ble_hs_cfg.store_status_arg = NULL;
}

esp_err_t bl_initialize_gatt_server(void) {
    ble_svc_gap_init();
    ble_svc_gatt_init();

    if (ble_gatts_count_cfg(BL_GATT_SERVICES) != 0) {
        return ESP_FAIL;
    }

    if (ble_gatts_add_svcs(BL_GATT_SERVICES) != 0) {
        return ESP_FAIL;
    }

    if (ble_svc_gap_device_name_set(TAG) != 0) {
        return ESP_FAIL;
    }

    if (ble_svc_gap_device_appearance_set(BL_VALUE_APPEARANCE_KEYBOARD) != 0) {
        return ESP_FAIL;
    };

    return ESP_OK;
}

void bl_task_run_nimble(void *param) {
    nimble_port_run();
    nimble_port_freertos_deinit();
}

void bl_start(void) {
    memset(bl_val_handle_list, 0, sizeof(bl_val_handle_list));

    bl_queue_input = NULL;
    bl_queue_keyboard = xQueueCreate(QUEUE_LENGTH, sizeof(hid_keyboard_report_t));
    memset(&bl_report_keyboard, 0, sizeof(bl_report_keyboard));
    bl_is_connected = false;
    bl_is_suspended = false;
    bl_conn_handle = 0;

    ESP_ERROR_CHECK(bl_initialize_nvs_flash());
    ESP_ERROR_CHECK(nimble_port_init());
    bl_initialize_ble_hs_cfg();
    ESP_ERROR_CHECK(bl_initialize_gatt_server());
    ble_store_config_init();
    nimble_port_freertos_init(bl_task_run_nimble);
}

void bl_task_transmit_data(void *param) {
    task_data_t *task_data = param;
    bl_queue_input = task_data->queue;

    while (true) {
        if (!bl_is_connected || bl_is_suspended || uxQueueMessagesWaiting(bl_queue_input) == 0) {
            vTaskDelay(pdMS_TO_TICKS(BL_LOOP_WAIT_MS));
            continue;
        }

        queue_data_t data;
        if (xQueueReceive(bl_queue_input, &data, 0) != pdTRUE) {
            ESP_LOGE(BL_TAG, "Receive data from the queue failed");
            continue;
        }

        if (data.key_pressed) {
            array_uint8_add(bl_report_keyboard.keycode, 6, data.keycode_low);
        } else {
            array_uint8_remove(bl_report_keyboard.keycode, 6, data.keycode_low);
        }

        if (xQueueSend(bl_queue_keyboard, &bl_report_keyboard, 0) != pdTRUE) {
            ESP_LOGE(BL_TAG, "Send keyboard report to the queue failed");
            continue;
        }

        if (ble_gatts_notify(bl_conn_handle, bl_val_handle_list[BL_INDEX_CHR_REPORT_KEYBOARD_INPUT]) != 0) {
            ESP_LOGE(BL_TAG,
                "Notify to report characteristic (keyboard input) failed, val_handle = %d",
                bl_val_handle_list[BL_INDEX_CHR_REPORT_KEYBOARD_INPUT]);
            continue;
        }
    }
}
