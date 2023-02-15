# MCU name
# Pro Micro (atmega32u4): max 28672 bytes
MCU = atmega32u4

# Bootloader selection
BOOTLOADER = caterina

# Build Options
#   change yes to no to disable
#
BOOTMAGIC_ENABLE = yes      # Enable Bootmagic Lite
MOUSEKEY_ENABLE = yes       # Mouse keys (1600 - 1618 bytes)
EXTRAKEY_ENABLE = yes       # Audio control and System control (298 - 388 bytes)
CONSOLE_ENABLE = no         # Console for debug (3386 - 3652 bytes)
COMMAND_ENABLE = no         # Commands for debug and configuration
NKRO_ENABLE = no            # Enable N-Key Rollover
BACKLIGHT_ENABLE = no       # Enable keyboard backlight functionality
RGBLIGHT_ENABLE = yes       # Enable keyboard RGB underglow
AUDIO_ENABLE = no           # Audio output

ENCODER_ENABLE = yes

CUSTOM_MATRIX = lite

SRC += matrix.c
SRC += extra/utility.c
SRC += extra/minesweeper.c
