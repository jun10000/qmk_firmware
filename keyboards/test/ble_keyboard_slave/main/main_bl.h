#pragma once

#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "host/ble_hs.h"
#include "host/ble_sm.h"
// #include "utility_led.h"

static const char *BL_TAG = "ble-keyboard-bl";

// to do
static const struct ble_gatt_svc_def BL_SERVICES[] = {
    {
        .type = ???,
        .uuid = ???,
        .includes = NULL,
        .characteristics = ???,
    },
    {
        0,
    },
};



//
// ble_hs_cfg callback functions
//

// to do
void ble_hs_cfg_gatts_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg) {

}

// to do
void ble_hs_cfg_reset_cb(int reason) {

}

// to do
void ble_hs_cfg_sync_cb(void) {

}

// to do
int ble_hs_cfg_store_status_cb(struct ble_store_status_event *event, void *arg) {

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
    ble_hs_cfg.sm_our_key_dist = BLE_SM_PAIR_KEY_DIST_ID | BLE_SM_PAIR_KEY_DIST_SIGN;
    ble_hs_cfg.sm_their_key_dist = BLE_SM_PAIR_KEY_DIST_ID | BLE_SM_PAIR_KEY_DIST_SIGN;
    ble_hs_cfg.reset_cb = ble_hs_cfg_reset_cb;
    ble_hs_cfg.sync_cb = ble_hs_cfg_sync_cb;

    // "ble_store_config_init" will be called
    // ble_hs_cfg.store_read_cb = NULL;
    // ble_hs_cfg.store_write_cb = NULL;
    // ble_hs_cfg.store_delete_cb = NULL;

    ble_hs_cfg.store_status_cb = ble_hs_cfg_store_status_cb;
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
