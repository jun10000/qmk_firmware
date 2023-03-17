#include "quantum.h"
#include "i2c_master.h"
#include "spi_master.h"
#include "uart.h"

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
#if IF_METHOD == IFM_I2C
    i2c_init();
#elif IF_METHOD == IFM_SPI
    spi_init();
#elif IF_METHOD == IFM_UART
    uart_init(UART_BAUDRATE);
#endif
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
#if IF_METHOD == IFM_I2C
    uint8_t data[I2C_DATA_LENGTH] = {0};
    data[0] = 0xFF;
    data[1] = HighByte(keycode);
    data[2] = LowByte(keycode);
    data[3] = record->event.pressed;
    data[4] = Checksum(data + 1, 3);

    if (i2c_start(I2C_ADDRESS) == I2C_STATUS_SUCCESS) {
        i2c_transmit(I2C_ADDRESS, data, I2C_DATA_LENGTH, I2C_TIMEOUT);
        i2c_stop();
    }
#elif IF_METHOD == IFM_SPI
    uint8_t data[SPI_DATA_LENGTH];
    data[0] = HighByte(keycode);
    data[1] = LowByte(keycode);
    data[2] = record->event.pressed;
    data[3] = Checksum(data + 0, 3);

    if (spi_start(SPI_SS_PIN, false, SPI_MODE, SPI_CLOCK_DIV)) {
        spi_transmit(data, SPI_DATA_LENGTH);
        spi_stop();
    }
#elif IF_METHOD == IFM_UART
    uint8_t data[UART_DATA_LENGTH];
    data[0] = HighByte(keycode);
    data[1] = LowByte(keycode);
    data[2] = record->event.pressed;

    uart_transmit(data, UART_DATA_LENGTH);
#endif

    return true;
}
