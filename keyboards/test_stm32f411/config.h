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
// IFM_I2C: I2C
#define IF_METHOD IFM_I2C

#if IF_METHOD == IFM_I2C
    #define I2C_DRIVER          I2CD3
    #define I2C1_SCL_PIN        A8
    #define I2C1_SCL_PAL_MODE   4
    #define I2C1_SDA_PIN        C9
    #define I2C1_SDA_PAL_MODE   4
    #define I2C1_OPMODE         OPMODE_I2C
    #define I2C1_CLOCK_SPEED    100000
    #define I2C1_DUTY_CYCLE     STD_DUTY_CYCLE

    #define I2C3_ESP32_ADDR     (0x55 << 1)
    #define I2C3_ESP32_TIMEOUT  200
    #define I2C3_ESP32_SENDLEN  128
#endif
