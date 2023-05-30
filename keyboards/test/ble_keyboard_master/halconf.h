#pragma once

#if IF_METHOD == IFM_I2C
    #define HAL_USE_I2C TRUE
#elif IF_METHOD == IFM_SPI
    #define HAL_USE_SPI TRUE
    #define SPI_USE_WAIT TRUE
    #define SPI_SELECT_MODE SPI_SELECT_MODE_PAD
#elif IF_METHOD == IFM_UART
    #define HAL_USE_SERIAL TRUE
#endif

#include_next <halconf.h>
