#pragma once

static const char *UTIL_TAG = "ble-keyboard-utility";

uint8_t get_checksum(uint8_t *bytes, int length) {
    uint8_t sum = 0x00;
    for (int i = 0; i < length; i++) {
        sum += bytes[i];
    }

    return sum;
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

void nimble_print_ble_gap_conn_desc(struct ble_gap_conn_desc *desc) {
    ESP_LOGI(UTIL_TAG,
        "(struct ble_gap_conn_desc) {\n"
        "   .sec_state.encrypted = %d,\n"
        "   .sec_state.authenticated = %d,\n"
        "   .sec_state.bonded = %d,\n"
        "   .sec_state.key_size = %d,\n"
        "   .our_id_addr.type = %d,\n"
        "   .our_id_addr.val = " MACSTR ",\n"
        "   .peer_id_addr.type = %d,\n"
        "   .peer_id_addr.val = " MACSTR ",\n"
        "   .our_ota_addr.type = %d,\n"
        "   .our_ota_addr.val = " MACSTR ",\n"
        "   .peer_ota_addr.type = %d,\n"
        "   .peer_ota_addr.val = " MACSTR ",\n"
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

int nimble_mbuf_read(const struct os_mbuf *om, void *dst, uint16_t exp_len) {
    if (OS_MBUF_PKTLEN(om) != exp_len) {
        return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
    }
    
    if (ble_hs_mbuf_to_flat(om, dst, exp_len, NULL) != 0) {
        return BLE_ATT_ERR_UNLIKELY;
    }
    
    return 0;
}

int nimble_mbuf_write(struct os_mbuf *om, const void *src, uint16_t len) {
    if (os_mbuf_append(om, src, len) != 0) {
        return BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    return 0;
}
