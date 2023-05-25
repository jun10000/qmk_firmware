#pragma once

void bl_gap_start_advertising(void);

int bl_gap_event_connect(int status, uint16_t conn_handle) {
    if (status != 0) {
        ESP_LOGE(BL_TAG, "Connection failed, status = 0x%x", status);
        bl_gap_start_advertising();
        return 0;
    }

    struct ble_gap_conn_desc desc;
    ESP_ERROR_CHECK(ble_gap_conn_find(conn_handle, &desc));
    ESP_LOGI(BL_TAG, "Connection established");
    nimble_print_ble_gap_conn_desc(&desc);
    
    xQueueReset(queue_input_if);
    xQueueReset(bl_queue_keyboard);
    memset(&bl_report_keyboard, 0, sizeof(bl_report_keyboard));
    bl_is_connected = true;
    bl_is_suspended = false;
    bl_conn_handle = desc.conn_handle;

    return 0;
}

int bl_gap_event_disconnect(int reason, struct ble_gap_conn_desc *conn) {
    ESP_LOGI(BL_TAG, "Disconnected, reason = 0x%x", reason);
    nimble_print_ble_gap_conn_desc(conn);

    xQueueReset(queue_input_if);
    xQueueReset(bl_queue_keyboard);
    memset(&bl_report_keyboard, 0, sizeof(bl_report_keyboard));
    bl_is_connected = false;
    bl_is_suspended = false;
    bl_conn_handle = 0;

    bl_gap_start_advertising();
    
    return 0;
}

int bl_gap_event_conn_update(int status, uint16_t conn_handle) {
    struct ble_gap_conn_desc desc;

    ESP_LOGI(BL_TAG, "Connection updated, status = 0x%x", status);
    ESP_ERROR_CHECK(ble_gap_conn_find(conn_handle, &desc));
    nimble_print_ble_gap_conn_desc(&desc);

    return 0;
}

int bl_gap_event_conn_update_req(const struct ble_gap_upd_params *peer_params,
                                 struct ble_gap_upd_params *self_params, uint16_t conn_handle) {
    ESP_LOGI(BL_TAG, "Connection update requested");

    return 0;
}

int bl_gap_event_adv_complete(int reason) {
    ESP_LOGI(BL_TAG, "Advertise completed, reason = 0x%x", reason);
    bl_gap_start_advertising();

    return 0;
}

int bl_gap_event_enc_change(int status, uint16_t conn_handle) {
    struct ble_gap_conn_desc desc;

    ESP_LOGI(BL_TAG, "Encryption changed, status = 0x%x", status);
    ESP_ERROR_CHECK(ble_gap_conn_find(conn_handle, &desc));
    nimble_print_ble_gap_conn_desc(&desc);

    return 0;
}

int bl_gap_event_passkey_action(struct ble_gap_passkey_params *params, uint16_t conn_handle) {
    ESP_LOGI(BL_TAG, "Passkey action started");

    switch (params->action) {
        case BLE_SM_IOACT_NONE:
            break;
        // case BLE_SM_IOACT_OOB:
        //     break;
        // case BLE_SM_IOACT_INPUT:
        //     break;
        // case BLE_SM_IOACT_DISP:
        //     break;
        // case BLE_SM_IOACT_NUMCMP:
        //     break;
        // case BLE_SM_IOACT_OOB_SC:
        //     break;
        default:
            ESP_LOGE(BL_TAG, "Detected bonding is not supported, action = 0x%x", params->action);
            break;
    }

    ESP_LOGI(BL_TAG, "Passkey action finished");

    return 0;
}

int bl_gap_event_notify_tx(int status, uint16_t conn_handle, uint16_t attr_handle, uint8_t indication) {
    ESP_LOGI(BL_TAG,
        "TX notification / indication received, "
        "status = 0x%x, conn_handle = 0x%x, attr_handle = 0x%x, indication = 0x%x",
        status, conn_handle, attr_handle, indication);
    
    return 0;
}

int bl_gap_event_subscribe(uint16_t conn_handle, uint16_t attr_handle, uint8_t reason, uint8_t prev_notify,
                           uint8_t cur_notify, uint8_t prev_indicate, uint8_t cur_indicate) {
    ESP_LOGI(BL_TAG,
        "Subscribed: conn_handle = 0x%x, attr_handle = 0x%x, reason = 0x%x, prev_notify = 0x%x, "
        "cur_notify = 0x%x, prev_indicate = 0x%x, cur_indicate = 0x%x",
        conn_handle, attr_handle, reason, prev_notify, cur_notify, prev_indicate, cur_indicate);

    return 0;
}

int bl_gap_event_mtu(uint16_t conn_handle, uint16_t channel_id, uint16_t value) {
    ESP_LOGI(BL_TAG, "MTU updated, conn_handle = 0x%x, channel_id = 0x%x, value = %d",
        conn_handle, channel_id, value);
    
    return 0;
}

int bl_gap_event_repeat_pairing(struct ble_gap_repeat_pairing *repeat_pairing) {
    struct ble_gap_conn_desc desc;

    ESP_ERROR_CHECK(ble_gap_conn_find(repeat_pairing->conn_handle, &desc));
    ble_store_util_delete_peer(&desc.peer_id_addr);

    return BLE_GAP_REPEAT_PAIRING_RETRY;
}

int ble_gap_event_cb(struct ble_gap_event *event, void *arg) {
    switch (event->type) {
        case BLE_GAP_EVENT_CONNECT:
            return bl_gap_event_connect(event->connect.status, event->connect.conn_handle);
        case BLE_GAP_EVENT_DISCONNECT:
            return bl_gap_event_disconnect(event->disconnect.reason, &event->disconnect.conn);
        case BLE_GAP_EVENT_CONN_UPDATE:
            return bl_gap_event_conn_update(event->conn_update.status, event->conn_update.conn_handle);
        case BLE_GAP_EVENT_CONN_UPDATE_REQ:
            return bl_gap_event_conn_update_req(event->conn_update_req.peer_params,
                event->conn_update_req.self_params, event->conn_update_req.conn_handle);
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
            return bl_gap_event_passkey_action(&event->passkey.params, event->passkey.conn_handle);
        // case BLE_GAP_EVENT_NOTIFY_RX:
        //     break;
        case BLE_GAP_EVENT_NOTIFY_TX:
            return bl_gap_event_notify_tx(event->notify_tx.status, event->notify_tx.conn_handle,
                event->notify_tx.attr_handle, event->notify_tx.indication);
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
            ESP_LOGI(BL_TAG, "Other GAP event is occured, type = 0x%x", event->type);
            break;
    }

    return 0;
}

void bl_gap_start_advertising(void) {
    ble_uuid16_t uuids[] = {
        BLE_UUID16_INIT(BL_UUID_SERVICE_HID),
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
        .name = (uint8_t *)name,
        .name_len = strlen(name),
        .name_is_complete = 1,
        .tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO,
        .tx_pwr_lvl_is_present = 1,
        .slave_itvl_range = NULL,
        .svc_data_uuid16 = NULL,
        .svc_data_uuid16_len = 0,
        .public_tgt_addr = NULL,
        .num_public_tgt_addrs = 0,
        .appearance = 0,
        .appearance_is_present = 0,
        .adv_itvl = 0,
        .adv_itvl_is_present = 0,
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
        ESP_LOGE(BL_TAG, "Set advertisement data failed, result = 0x%x", result);
        return;
    }

    result = ble_gap_adv_start(bl_address_type, NULL, BLE_HS_FOREVER, &params, ble_gap_event_cb, NULL);
    if (result != 0) {
        ESP_LOGE(BL_TAG, "Start advertising failed, result = 0x%x", result);
        return;
    }
}
