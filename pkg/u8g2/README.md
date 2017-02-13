# U8g2

## Introduction
[U8g2](https://github.com/olikraus/u8g2) is a monochrome graphics library for LCDs and OLEDs. It contains both drivers and high-level drawing routines.

The library is originally written for Arduino boards, but it runs just fine on other platforms, as long as the right drivers are available.

## Usage
Just put `USEPKG += u8g2` in your Makefile and `#include "u8g2.h"` to your code. Refer to the [U8g2 wiki](https://github.com/olikraus/u8g2/wiki) for more information on the API.

## RIOT-OS interface
This package patches the original source to add an interface for RIOT-OS.

The following two callbacks add support for the included drivers via I2C and SPI peripherals:

* `u8x8_byte_riotos_hw_spi`
* `u8x8_byte_riotos_hw_i2c`

For timing and GPIO related operations, the following callback is available.

* `u8x8_gpio_and_delay_riotos`

U8g2 needs to map pin numbers to RIOT-OS pin numbers. It also needs to know which peripheral to use. The following two methods can be used to set this information.

* `u8g2_SetPins(u8g2_dev, pins, bitmap)`
* `u8g2_SetDevice(u8g2_dev, dev)`

Note: `pins` should point to `gpio_t` array of U8g2 pin numbers to RIOT-OS pins. Due to this, `pins` can take up an additional 100 bytes, because it will use memory for the pins you do not map. You can overcome this limitation by implementing `u8x8_gpio_and_delay_riotos` yourself and hardcode the pins.

### Example
```
u8g2_t u8g2;

gpio_t pins[] = {
    [U8X8_PIN_CS] = GPIO(PA, 0),
    [U8X8_PIN_DC] = GPIO(PA, 1),
    [U8X8_PIN_RESET] = GPIO(PA, 2)
};

uint32_t bitmap = (
    (1 << U8X8_PIN_CS) +
    (1 << U8X8_PIN_DC) +
    (1 << U8X8_PIN_RESET)
);

u8g2_Setup_ssd1306_128x64_noname_1(&u8g2, U8G2_R0, u8x8_byte_riotos_hw_spi, u8x8_gpio_and_delay_riotos);

u8g2_SetPins(&u8g2, pins, bitmap);
u8g2_SetDevice(&u8g2, SPI_DEV(0));
```

## Virtual displays
For targets without an I2C or SPI, virtual displays are available. These displays are part of U8g2, but are not compiled by default.

* By adding `USEMODULE += u8g2_utf8`, a terminal display is used as virtual display, using UTF8 block characters that are printed to stdout.
* By adding `USEMODULE += u8g2_sdl`, a SDL virtual display will be used. This is only available on native targets that have SDL installed. It uses `sdl-config` to find the headers and libraries. Note that RIOT-OS builds 32-bit binaries and requires 32-bit SDL libraries.

### Example
```
u8g2_t u8g2;

u8g2_SetupBuffer_Utf8(&u8g2, U8G2_R0);
```
