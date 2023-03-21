#pragma once

#include_next <mcuconf.h>

#if IF_METHOD == IFM_I2C
    #undef STM32_I2C_USE_I2C3
    #define STM32_I2C_USE_I2C3 TRUE
#elif IF_METHOD == IFM_SPI
    #undef STM32_SPI_USE_SPI2
    #define STM32_SPI_USE_SPI2 TRUE
#elif IF_METHOD == IFM_UART
    #undef STM32_SERIAL_USE_USART1
    #define STM32_SERIAL_USE_USART1 TRUE
#endif
