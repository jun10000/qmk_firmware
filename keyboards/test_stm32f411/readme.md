# test_stm32f411

![test_stm32f411](imgur.com image replace me!)

*A short description of the keyboard/project*

* Keyboard Maintainer: [jun10000](https://github.com/jun10000)
* Hardware Supported: *The PCBs, controllers supported*
* Hardware Availability: *Links to where you can find this hardware*

Make example for this keyboard (after setting up your build environment):

    make test_stm32f411:default

Flashing example for this keyboard:

    make test_stm32f411:default:flash

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## Bootloader

Enter the bootloader in 3 ways:

* **Bootmagic reset**: Hold down the key at (0,0) in the matrix (usually the top left key or Escape) and plug in the keyboard
* **Physical reset button**: Briefly press the button on the back of the PCB - some may have pads you must short instead
* **Keycode in layout**: Press the key mapped to `QK_BOOT` if it is available

## Boot config

QMK Firmware uses DFU boot.  
Please connect BOOT0 to VDD, and PB2 (BOOT1) to GND.

## Interface test

### Wire connection - Common stm32f411

PC2 - Button (KC_1) - GND  
PC3 - Button (KC_2) - GND

### Wire connection - I2C

|  Pin|  stm32f411|  esp32-s3|
|-----|-----------|----------|
|  SDA|        PC9|         4|
|  SCL|        PA8|         5|

#### Note

stm32f411 uses I2C3, AF04.  
4.7 kΩ pull-up resistors are connected to SDA and SCL pins.  

### Wire connection - SPI

|   Pin|  stm32f411|  esp32-s3|
|------|-----------|----------|
|    CS|       PB12|        10|
|  SCLK|       PB13|        12|
|  MOSI|       PB15|        11|
|  MISO|       PB14|        13|

#### Note

stm32f411 uses SPI2, AF05.  
esp32-s3 uses SPI2 (FSPI).  
