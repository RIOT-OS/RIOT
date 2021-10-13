# Ucglib

## Introduction
[Ucglib](https://github.com/olikraus/ucglib) is a color graphics library for LCDs and OLEDs. It contains both drivers and high-level drawing routines.

The library is originally written for Arduino boards, but it runs just fine on other platforms, as long as the right drivers are available.

## Usage
Just put `USEPKG += ucglib` in your Makefile and `#include "ucg.h"` to your code. Refer to the [Ucglib wiki](https://github.com/olikraus/ucglib/wiki) for more information on the API.

## RIOT-OS interface
This package patches the original source to add an interface for RIOT-OS.

Only the callback for SPI peripherals is supported:

* `ucg_com_hw_spi_riotos`

These methods require a structure containing peripheral information (`ucg_riotos_t`), that is set using the `ucg_SetUserPtr` function. This structure contains the peripheral and pin mapping.

If the above interface is not sufficient, it is still possible to write a dedicated interface by (re-)implementing the methods above. Refer to the [Ucglib wiki](https://github.com/olikraus/ucglib/wiki) for more information.

### Example
```c
ucg_t ucg;

ucg_riotos_t user_data =
{
    .device_index = SPI_DEV(0),
    .pin_cs = GPIO_PIN(PA, 0),
    .pin_cd = GPIO_PIN(PA, 1),
    .pin_reset = GPIO_PIN(PA, 2)
};

ucg_SetUserPtr(&ucg, &user_data);

ucg_Init(&ucg, ucg_dev_ssd1331_18x96x64_univision, ucg_ext_ssd1331_18, ucg_com_hw_spi_riotos);
```

## Virtual displays
For targets without SPI, a virtual display is available. Support for a virtual display is not compiled by default.

* By adding `USEMODULE += ucglib_sdl`, a SDL virtual display will be used. This is only available on native targets that have SDL2 installed. It uses `sdl2-config` to find the headers and libraries. Note that RIOT-OS builds 32-bit binaries and requires 32-bit SDL2 libraries.

### Example
```c
ucg_t ucg;

ucg_Init(&ucg, ucg_sdl_dev_cb, ucg_ext_none, NULL);
```
