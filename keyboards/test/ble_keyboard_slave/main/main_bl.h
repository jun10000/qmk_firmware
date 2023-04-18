#pragma once

#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "host/ble_hs.h"
// #include "utility_led.h"

#define BL_UUID_GATT_SERVICE_BATTERY                0x180F
#define BL_UUID_GATT_SERVICE_DEVICE_INFORMATION     0x180A
#define BL_UUID_GATT_SERVICE_HID                    0x1812

static const char *BL_TAG = "ble-keyboard-bl";

// to do
static const struct ble_gatt_svc_def BL_SERVICE_BATTERY = {
    .type = BLE_GATT_SVC_TYPE_PRIMARY,
    .uuid = BLE_UUID16_DECLARE(BL_UUID_GATT_SERVICE_BATTERY),
    .includes = NULL,
    .characteristics = ???,
};

// to do
static const struct ble_gatt_svc_def BL_SERVICE_DEVICE_INFORMATION = {
    .type = BLE_GATT_SVC_TYPE_PRIMARY,
    .uuid = BLE_UUID16_DECLARE(BL_UUID_GATT_SERVICE_DEVICE_INFORMATION),
    .includes = NULL,
    .characteristics = ???,
};

// to do
static const struct ble_gatt_svc_def BL_SERVICE_HID = {
    .type = BLE_GATT_SVC_TYPE_PRIMARY,
    .uuid = BLE_UUID16_DECLARE(BL_UUID_GATT_SERVICE_HID),
    .includes = {
        &BL_SERVICE_BATTERY,
        &BL_SERVICE_DEVICE_INFORMATION,
        NULL,
    },
    .characteristics = ???,
};

static const struct ble_gatt_svc_def BL_SERVICES[] = {
    BL_SERVICE_HID,
    {0},
};

static uint8_t bl_address_type;



//
// User side functions 2
//

// to do
void bl_advertise(void) {

}



//
// ble_hs_cfg callback functions
//

void ble_hs_cfg_gatts_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg) {
    char tmp[BLE_UUID_STR_LEN];

    switch (ctxt->op) {
        case BLE_GATT_REGISTER_OP_SVC:
            ESP_LOGI(BL_TAG, "Service %s (handle = %d) is registered",
                ble_uuid_to_str(ctxt->svc_def->uuid, tmp),
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
    ESP_ERROR_CHECK(ble_hs_util_ensure_rand_addr());

    int result = ble_hs_id_infer_auto(1, &bl_address_type);
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

    ESP_LOGI(BL_TAG, "Device address = "MACSTR, MAC2STR(address));
    bl_advertise();
}



//
// User side functions 1
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
    ble_hs_cfg.sm_our_key_dist = BLE_SM_PAIR_KEY_DIST_ID | BLE_SM_PAIR_KEY_DIST_SIGN;
    ble_hs_cfg.sm_their_key_dist = BLE_SM_PAIR_KEY_DIST_ID | BLE_SM_PAIR_KEY_DIST_SIGN;
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

    if (ble_gatts_count_cfg(BL_SERVICES) != 0) {
        return ESP_FAIL;
    }

    if (ble_gatts_add_svcs(BL_SERVICES) != 0) {
        return ESP_FAIL;
    }

    if (ble_svc_gap_device_name_set(TAG) != 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

void bl_task_run_nimble(void *param) {
    nimble_port_run();
    nimble_port_freertos_deinit();
}

void bl_start(void) {
    ESP_ERROR_CHECK(bl_initialize_nvs_flash());
    ESP_ERROR_CHECK(esp_nimble_hci_init());
    ESP_ERROR_CHECK(nimble_port_init());
    bl_initialize_ble_hs_cfg();
    ESP_ERROR_CHECK(bl_initialize_gatt_server());
    ble_store_config_init();
    nimble_port_freertos_init(bl_task_run_nimble);
}

// to do
void bl_task_transmit_data(void *param) {
    task_data_t *task_data = param;
    queue_data_t data;

    // while (true) {
    //     if (!tud_mounted() || uxQueueMessagesWaiting(task_data->queue) == 0) {
    //         vTaskDelay(pdMS_TO_TICKS(BL_LOOP_WAIT_MS));
    //         continue;
    //     }

    //     if (xQueueReceive(task_data->queue, &data, 0) != pdTRUE) {
    //         ESP_LOGE(BL_TAG, "Receive data from the queue failed");
    //         continue;
    //     }

    //     if (data.key_pressed) {
    //         uint8_t send_keycodes[6] = { data.keycode_low };
    //         if (!tud_hid_keyboard_report(HID_ITF_PROTOCOL_KEYBOARD, 0, send_keycodes)) {
    //             ESP_LOGE(BL_TAG, "Send data to host failed");
    //         }
    //     } else {
    //         if (!tud_hid_keyboard_report(HID_ITF_PROTOCOL_KEYBOARD, 0, NULL)) {
    //             ESP_LOGE(BL_TAG, "Send data to host failed");
    //         }
    //     }
    // }
}
