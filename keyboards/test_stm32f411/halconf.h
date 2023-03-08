#pragma once

#if IF_METHOD == IFM_I2C
    #define HAL_USE_I2C TRUE
#endif

#include_next <halconf.h>
