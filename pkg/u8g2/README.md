# U8g2

## Introduction
[U8g2](https://github.com/olikraus/u8g2) is a monochrome graphics library for LCDs and OLEDs. It contains both drivers and high-level drawing routines.

The library is originally written for Arduino boards, but it runs just fine on other platforms, as long as the right drivers are available.

## Usage
Just put `USEPKG += u8g2` in your Makefile and `#include "u8g2.h"` into your code. Refer to the [U8g2 wiki](https://github.com/olikraus/u8g2/wiki) for more information on the API.

## RIOT-OS interface
This adds an interface for RIOT-OS.

The following two callbacks add support for the included drivers via I2C and SPI peripherals:

* `u8x8_byte_hw_spi_riotos`
* `u8x8_byte_hw_i2c_riotos`

For timing and GPIO related operations, the following callback is available.

* `u8x8_gpio_and_delay_riotos`

These methods require a structure containing peripheral information (`u8x8_riotos_t`), that is set using the `u8g2_SetUserPtr` function. This structure contains the peripheral and pin mapping.

If the above interface is not sufficient, it is still possible to write a dedicated interface by (re-)implementing the methods above. Refer to the [U8g2 wiki](https://github.com/olikraus/u8g2/wiki) for more information.

### Example
```c
u8g2_t u8g2;

u8x8_riotos_t user_data =
{
    .device_index = SPI_DEV(0),
    .pin_cs = GPIO_PIN(PA, 0),
    .pin_dc = GPIO_PIN(PA, 1),
    .pin_reset = GPIO_PIN(PA, 2)
};

u8g2_SetUserPtr(&u8g2, &user_data);

u8g2_Setup_ssd1306_128x64_noname_1(&u8g2, U8G2_R0, u8x8_byte_riotos_hw_spi, u8x8_gpio_and_delay_riotos);
```

## Virtual displays
For targets without an I2C or SPI, virtual displays are available. These displays are part of U8g2, but are not compiled by default.

* By adding `USEMODULE += u8g2_utf8`, a terminal display is used as virtual display, using UTF8 block characters that are printed to stdout.
* By adding `USEMODULE += u8g2_sdl`, a SDL virtual display will be used. This is only available on native targets that have SDL installed. It uses `sdl2-config` to find the headers and libraries. Note that RIOT-OS builds 32-bit binaries and requires 32-bit SDL libraries.

### Example
```c
u8g2_t u8g2;

u8g2_SetupBuffer_Utf8(&u8g2, U8G2_R0);
```
