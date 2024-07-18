/*
Copyright 2017 Danny Nguyen <danny@hexwire.com>
Copyright 2021 Salicylic_Acid
Copyright 2021 3araht
Copyright 2022 monksoffunk
Copyright 2022-2023 jun10000

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#define USE_SERIAL

#define RGBLED_NUM 18
#define RGBLIGHT_SLEEP

// If enabled, can't modify color settings with the via.
#define RGBLIGHT_LAYERS

#if defined(RGBLIGHT_ENABLE) && !defined(IOS_DEVICE_ENABLE)
  #define USB_MAX_POWER_CONSUMPTION 400
#else
  #define USB_MAX_POWER_CONSUMPTION 100
#endif

#define TAP_CODE_DELAY 10

#undef DEBOUNCE
#define DEBOUNCE 10
