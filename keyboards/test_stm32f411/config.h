// Copyright 2023 jun10000 (@jun10000)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

/*
 * Feature disable options
 *  These options are also useful to firmware size reduction.
 */

/* disable debug print */
//#define NO_DEBUG

/* disable print */
//#define NO_PRINT

/* disable action features */
//#define NO_ACTION_LAYER
//#define NO_ACTION_TAPPING
//#define NO_ACTION_ONESHOT

// Interface method switcher
// Make sure to rewrite rules.mk too
#define IFM_I2C     0           // I2C
#define IFM_SPI     1           // SPI
#define IFM_UART    2           // UART
#define IF_METHOD   IFM_UART

#if IF_METHOD == IFM_I2C
    #define I2C_DRIVER          I2CD3
    #define I2C1_SCL_PIN        A8
    #define I2C1_SCL_PAL_MODE   4
    #define I2C1_SDA_PIN        C9
    #define I2C1_SDA_PAL_MODE   4
    #define I2C1_OPMODE         OPMODE_I2C
    #define I2C1_CLOCK_SPEED    100000
    #define I2C1_DUTY_CYCLE     STD_DUTY_CYCLE

    #define I2C_ADDRESS         (0x55 << 1)
    #define I2C_TIMEOUT         200
    #define I2C_DATA_LENGTH     128
#elif IF_METHOD == IFM_SPI
    #define SPI_DRIVER          SPID2
    #define SPI_SCK_PIN         B13
    #define SPI_SCK_PAL_MODE    5
    #define SPI_MOSI_PIN        B15
    #define SPI_MOSI_PAL_MODE   5
    #define SPI_MISO_PIN        B14
    #define SPI_MISO_PAL_MODE   5

    #define SPI_SS_PIN          B12
    #define SPI_MODE            0       // CPOL * 2 + CPHA
    #define SPI_CLOCK_DIV       2       // Max frequency: 50MHz (SPI1, 4, 5), 25MHz (SPI2, 3)
                                        // Available value: 2 - 256
    #define SPI_DATA_LENGTH     4
#elif IF_METHOD == IFM_UART
    #define SERIAL_DRIVER       SD1
    #define SD1_TX_PIN          B6
    #define SD1_TX_PAL_MODE     7
    #define SD1_RX_PIN          B7
    #define SD1_RX_PAL_MODE     7

    #define UART_BAUDRATE       115200  // 8μs
    #define UART_DATA_LENGTH    4
#endif
