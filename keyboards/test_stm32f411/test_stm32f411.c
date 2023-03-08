#include "quantum.h"
#include "i2c_master.h"

uint8_t LowByte(uint16_t u16) {
    return (uint8_t)u16;
}

uint8_t HighByte(uint16_t u16) {
    return (uint8_t)(u16 >> 8);
}

uint8_t Checksum(uint8_t *bytes, int length) {
    uint8_t sum = 0x00;
    for (int i = 0; i < length; i++) {
        sum += bytes[i];
    }
    return sum;
}

void keyboard_post_init_user(void) {
    // UART
    // uart_init(115200);
    i2c_init();
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    uint8_t data[I2C3_ESP32_SENDLEN] = {0};
    data[0] = 0xFF;
    data[1] = HighByte(keycode);
    data[2] = LowByte(keycode);
    data[3] = record->event.pressed;
    data[4] = Checksum(data + 1, 3);

    // UART
    // uart_transmit(data,5);
    // if(uart_available()){
    //     uart_read();
    // }

#if IF_METHOD == IFM_I2C
    if (i2c_start(I2C3_ESP32_ADDR) == I2C_STATUS_SUCCESS) {
        i2c_transmit(I2C3_ESP32_ADDR, data, I2C3_ESP32_SENDLEN, I2C3_ESP32_TIMEOUT);
        i2c_stop();
    }
#endif

    return true;
}
