# Ucglib

## Introduction
[Ucglib](https://github.com/olikraus/ucglib) is a color graphics library for LCDs and OLEDs. It contains both drivers and high-level drawing routines.

The library is originally written for Arduino boards, but it runs just fine on other platforms, as long as the right drivers are available.

## Usage
Just put `USEPKG += ucglib` in your Makefile and `#include "ucg.h"` to your code. Refer to the [Ucglib wiki](https://github.com/olikraus/ucglib/wiki) for more information on the API.

## RIOT-OS interface
This package patches the original source to add an interface for RIOT-OS.

Only the callback for SPI peripherals is supported:

* `ucg_com_riotos_hw_spi`

Ucglib needs to map pin numbers to RIOT-OS pin numbers. It also needs to know which peripheral to use. The following two methods can be used to set this information.

* `ucg_SetPins(ucg_dev, pins, bitmap)`
* `ucg_SetDevice(ucg_dev, dev)`

Note: `pins` should point to `gpio_t` array of Ucglib pin numbers to RIOT-OS pins. Due to this, `pins` can take up an additional 100 bytes, because it will use memory for the pins you do not map. You can overcome this limitation by implementing `ucg_com_riotos_hw_spi` yourself and hardcode the pins.

### Example
```
ucg_t ucg;

gpio_t pins[] = {
    [UCG_PIN_CS] = GPIO(PA, 0),
    [UCG_PIN_CD] = GPIO(PA, 1),
    [UCG_PIN_RESET] = GPIO(PA, 2)
};

uint32_t bitmap = (
    (1 << UCG_PIN_CS) +
    (1 << UCG_PIN_CD) +
    (1 << UCG_PIN_RESET)
);

ucg_SetPins(&ucg, pins, bitmap);
ucg_SetDevice(&ucg, SPI_DEV(0));

ucg_Init(&ucg, ucg_dev_ssd1331_18x96x64_univision, ucg_ext_ssd1331_18, ucg_com_riotos_hw_spi);
```

## Virtual displays
For targets without an I2C or SPI, a virtual display is available. Support for a virtual display is not compiled in by default.

* By adding `USEMODULE += ucglib_sdl`, a SDL virtual display will be used. This is only available on native targets that have SDL2 installed. It uses `sdl2-config` to find the headers and libraries. Note that RIOT-OS builds 32-bit binaries and requires 32-bit SDL2 libraries.

### Example
```
ucg_t ucg;

ucg_Init(&ucg, ucg_sdl_dev_cb, ucg_ext_none, NULL);
```
