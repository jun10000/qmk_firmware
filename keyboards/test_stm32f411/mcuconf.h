#pragma once

#include_next <mcuconf.h>

#if IF_METHOD == IFM_I2C
    #undef STM32_I2C_USE_I2C3
    #define STM32_I2C_USE_I2C3 TRUE
#endif
