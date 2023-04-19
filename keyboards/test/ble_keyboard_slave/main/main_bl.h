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
#define BL_APPEARANCE_HID_KEYBOARD                  0x03C1
#define BL_ADVERTISING_INTERVAL_MS                  40      // min: 20

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
// User side functions 3
//

void bl_print_ble_gap_conn_desc(struct ble_gap_conn_desc *desc) {
    ESP_LOGI(BL_TAG,
        "(struct ble_gap_conn_desc) {\n"
        "   .sec_state.encrypted = %d,\n"
        "   .sec_state.authenticated = %d,\n"
        "   .sec_state.bonded = %d,\n"
        "   .sec_state.key_size = %d,\n"
        "   .our_id_addr.type = %d,\n"
        "   .our_id_addr.val = "MACSTR",\n"
        "   .peer_id_addr.type = %d,\n"
        "   .peer_id_addr.val = "MACSTR",\n"
        "   .our_ota_addr.type = %d,\n"
        "   .our_ota_addr.val = "MACSTR",\n"
        "   .peer_ota_addr.type = %d,\n"
        "   .peer_ota_addr.val = "MACSTR",\n"
        "   .conn_handle = %d,\n"
        "   .conn_itvl = %d,\n"
        "   .conn_latency = %d,\n"
        "   .supervision_timeout = %d,\n"
        "   .role = %d,\n"
        "   .master_clock_accuracy = %d,\n"
        "}",
        desc->sec_state.encrypted,
        desc->sec_state.authenticated,
        desc->sec_state.bonded,
        desc->sec_state.key_size,
        desc->our_id_addr.type,
        MAC2STR(desc->our_id_addr.val),
        desc->peer_id_addr.type,
        MAC2STR(desc->peer_id_addr.val),
        desc->our_ota_addr.type,
        MAC2STR(desc->our_ota_addr.val),
        desc->peer_ota_addr.type,
        MAC2STR(desc->peer_ota_addr.val),
        desc->conn_handle,
        desc->conn_itvl,
        desc->conn_latency,
        desc->supervision_timeout,
        desc->role,
        desc->master_clock_accuracy);
}

void bl_start_advertising(void);

// to do: think hid_clean_vars
int bl_gap_event_connect(int status, uint16_t conn_handle) {
    struct ble_gap_conn_desc desc;

    if (status == 0) {
        ESP_LOGI(BL_TAG, "Connection established");
        ESP_ERROR_CHECK(ble_gap_conn_find(conn_handle, &desc));
        bl_print_ble_gap_conn_desc(&desc);
        // hid_clean_vars(&desc);
    } else {
        ESP_LOGE(BL_TAG, "Connection failed, status = %d", status);
        bl_start_advertising();
    }

    return 0;
}

// to do: think hid_set_disconnected
int bl_gap_event_disconnect(int reason, struct ble_gap_conn_desc *conn) {
    ESP_LOGI(BL_TAG, "Disconnected, reason = %d", reason);
    bl_print_ble_gap_conn_desc(conn);
    // hid_set_disconnected();
    bl_start_advertising();

    return 0;
}

// to do
int bl_gap_event_conn_update(int status, uint16_t conn_handle) {

}

// to do
int bl_gap_event_adv_complete(int reason) {

}

// to do
int bl_gap_event_enc_change(int status, uint16_t conn_handle) {

}

// to do
int bl_gap_event_passkey_action(struct ble_gap_passkey_params *params, uint16_t conn_handle) {

}

// to do
int bl_gap_event_subscribe(uint16_t conn_handle, uint16_t attr_handle, uint8_t reason, uint8_t prev_notify,
                           uint8_t cur_notify, uint8_t prev_indicate, uint8_t cur_indicate) {

}

// to do
int bl_gap_event_mtu(uint16_t conn_handle, uint16_t channel_id, uint16_t value) {

}

// to do
int bl_gap_event_repeat_pairing(struct ble_gap_repeat_pairing *repeat_pairing) {

}



//
// Callback functions 2
//

int ble_gap_event_cb(struct ble_gap_event *event, void *arg) {
    switch (event->type) {
        case BLE_GAP_EVENT_CONNECT:
            return bl_gap_event_connect(event->connect.status, event->connect.conn_handle);
        case BLE_GAP_EVENT_DISCONNECT:
            return bl_gap_event_disconnect(event->disconnect.reason, &event->disconnect.conn);
        case BLE_GAP_EVENT_CONN_UPDATE:
            return bl_gap_event_conn_update(event->conn_update.status, event->conn_update.conn_handle);
        // case BLE_GAP_EVENT_CONN_UPDATE_REQ:
        //     break;
        // case BLE_GAP_EVENT_L2CAP_UPDATE_REQ:
        //     break;
        // case BLE_GAP_EVENT_TERM_FAILURE:
        //     break;
        // case BLE_GAP_EVENT_DISC:
        //     break;
        // case BLE_GAP_EVENT_DISC_COMPLETE:
        //     break;
        case BLE_GAP_EVENT_ADV_COMPLETE:
            return bl_gap_event_adv_complete(event->adv_complete.reason);
        case BLE_GAP_EVENT_ENC_CHANGE:
            return bl_gap_event_enc_change(event->enc_change.status, event->enc_change.conn_handle);
        case BLE_GAP_EVENT_PASSKEY_ACTION:
            return bl_gap_event_passkey_action(event->passkey.params, event->passkey.conn_handle);
        // case BLE_GAP_EVENT_NOTIFY_RX:
        //     break;
        // case BLE_GAP_EVENT_NOTIFY_TX:
        //     break;
        case BLE_GAP_EVENT_SUBSCRIBE:
            return bl_gap_event_subscribe(event->subscribe.conn_handle, event->subscribe.attr_handle,
                event->subscribe.reason, event->subscribe.prev_notify, event->subscribe.cur_notify,
                event->subscribe.prev_indicate, event->subscribe.cur_indicate);
        case BLE_GAP_EVENT_MTU:
            return bl_gap_event_mtu(event->mtu.conn_handle, event->mtu.channel_id, event->mtu.value);
        // case BLE_GAP_EVENT_IDENTITY_RESOLVED:
        //     break;
        case BLE_GAP_EVENT_REPEAT_PAIRING:
            return bl_gap_event_repeat_pairing(&event->repeat_pairing);
        // case BLE_GAP_EVENT_PHY_UPDATE_COMPLETE:
        //     break;
        // case BLE_GAP_EVENT_EXT_DISC:
        //     break;
        // case BLE_GAP_EVENT_PERIODIC_SYNC:
        //     break;
        // case BLE_GAP_EVENT_PERIODIC_REPORT:
        //     break;
        // case BLE_GAP_EVENT_PERIODIC_SYNC_LOST:
        //     break;
        // case BLE_GAP_EVENT_SCAN_REQ_RCVD:
        //     break;
        // case BLE_GAP_EVENT_PERIODIC_TRANSFER:
        //     break;
        // case BLE_GAP_EVENT_PATHLOSS_THRESHOLD:
        //     break;
        // case BLE_GAP_EVENT_TRANSMIT_POWER:
        //     break;
        // case BLE_GAP_EVENT_SUBRATE_CHANGE:
        //     break;
        default:
            break;
    }

    return 0;
}



//
// User side functions 2
//

void bl_start_advertising(void) {
    ble_uuid16_t uuids[] = {
        BLE_UUID16_INIT(BL_UUID_GATT_SERVICE_HID),
    };
    const char *name = ble_svc_gap_device_name();

    struct ble_hs_adv_fields fields = {
        .flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP,
        .uuids16 = uuids,
        .num_uuids16 = sizeof(uuids),
        .uuids16_is_complete = 1,
        .uuids32 = NULL,
        .num_uuids32 = 0,
        .uuids32_is_complete = 0,
        .uuids128 = NULL,
        .num_uuids128 = 0,
        .uuids128_is_complete = 0,
        .name = name,
        .name_len = strlen(name),
        .name_is_complete = 1,
        .tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO,
        .tx_pwr_lvl_is_present = 1,
        .slave_itvl_range = NULL,
        .svc_data_uuid16 = NULL,
        .svc_data_uuid16_len = 0,
        .public_tgt_addr = NULL,
        .num_public_tgt_addrs = 0,
        .appearance = BL_APPEARANCE_HID_KEYBOARD,
        .appearance_is_present = 1,
        .adv_itvl = BL_ADVERTISING_INTERVAL_MS,
        .adv_itvl_is_present = 1,
        .svc_data_uuid32 = NULL,
        .svc_data_uuid32_len = 0,
        .svc_data_uuid128 = NULL,
        .svc_data_uuid128_len = 0,
        .uri = NULL,
        .uri_len = 0,
        .mfg_data = NULL,
        .mfg_data_len = 0,
    };

    struct ble_gap_adv_params params = {
        .conn_mode = BLE_GAP_CONN_MODE_UND,
        .disc_mode = BLE_GAP_DISC_MODE_GEN,
        .itvl_min = 0,
        .itvl_max = 0,
        .channel_map = 0,
        .filter_policy = 0,
        .high_duty_cycle = 0,
    };

    int result = ble_gap_adv_set_fields(&fields);
    if (result != 0) {
        ESP_LOGE(BL_TAG, "Set advertisement data failed, result = %d", result);
        return;
    }

    result = ble_gap_adv_start(bl_address_type, NULL, BLE_HS_FOREVER, &params, ble_gap_event_cb, NULL);
    if (result != 0) {
        ESP_LOGE(BL_TAG, "Start advertising failed, result = %d", result);
        return;
    }
}



//
// Callback functions 1
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
    bl_start_advertising();
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

    if (ble_svc_gap_device_appearance_set(BL_APPEARANCE_HID_KEYBOARD) != 0) {
        return ESP_FAIL;
    };

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
