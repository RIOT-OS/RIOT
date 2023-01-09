/*
 * Copyright (C) 2023 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_hip_badge HiP Badge
 * @ingroup     boards_esp32c3
 * @brief       Support for the Hacking in Parallel Badge
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

The HiP Badge is a board that was given to participants of the 2022 *Hacking in Parallel* event.

![Image of the HiP badge with custom frame and SAO (I2C) expander](https://camo.githubusercontent.com/834dbee4ed9a9a90480bd1a239e54718f3a9613b694d0a2e26a3b25e55ce2d82/68747470733a2f2f6d61747269782d636c69656e742e6d61747269782e6f72672f5f6d61747269782f6d656469612f72302f646f776e6c6f61642f6d6f6e65726f2e736f6369616c2f757351777a4f656957516e6671546247416845734b684a76)

# Features

 - ESP32-C3FH4AZ SoC
 - I2C via SAO headers
 - UART via pin headers
 - 16 WS2812B LEDs
 - USB-C Serial/JTAG
 - 3 User button / 1 Reset button
 - ST25DV04K NFC/I2C EEPROM (TODO: driver)

# Flashing the device

The device can be programmed using the USB-C Serial/JTAG feature.
Just plug in a cable to your computer and flash the device with

    make BOARD=hip-badge flash

This should automatically reset the ESP32C3 into bootloader mode and start the download process.
If for some reason this gets stuck or the badge does not register as a /dev/ttyACMx device, you
can manually force it into bootloader mode by pressing the reset button (SW3/blue) while holding
the BOOT button (SW4).
When doing this, the device will still be in bootloader mode after flashing, you have to press
the reset button (SW3/blue) again to start your application.

There is currently still a bug in the USB Serial stdio implementation: After flashing or a reboot,
there is no output. Do a manual reset (press the reset button) to restore output.

# Bugs

The event (and badge) were organized on short notice (6 Weeks), so there are some hardware issues:

 - No WiFi/BLE antenna. There is a pad on the back of the PCB where a socket was supposed to be, but
   nothing is connected. You can try solder something onto it yourself, but the trace is already long
   and winded.
   BLE works with the existing trace alone (on short range), WiFi/ESP Now does not.
 - IR transceiver broken. Can be fixed with a steady hand?
 - SGP30 air quality sensor is only populated on some boards (red case)
 - MAX17048 fuel gauge not populated

# Resources

 - [Badge Clinic](https://wiki.hip-berlin.de/index.php?title=Badge_Clinic)
 - [Design files](https://gitlab.com/tidklaas/hip-badge/)
