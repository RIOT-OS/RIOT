@defgroup pkg_u8g2 U8G2 graphic library for monochome displays
@ingroup pkg
@ingroup drivers_display

# U8G2

[U8G2](https://github.com/olikraus/u8g2) is a monochrome graphics library for
LCDs and OLEDs. It contains both drivers and high-level drawing routines. The
library is originally written for Arduino boards, but it runs just fine on
other platforms, as long as the right drivers are available.

## Supported Displays
For a complete list of supported displays, check the u8g2 documentation:
https://github.com/olikraus/u8g2/wiki/u8g2setupc#setup-function-reference

## RIOT Integration
You can use the package directly via its API (see
[this section](#direct-usage)), or via the integration to @ref drivers_disp_dev
(see [this section](#disp-dev-usage))

## Usage via Display Device Interface {#disp-dev-usage}
RIOT's @ref drivers_disp_dev abstracts different display drivers and provides
simplified access to them. This package is integrated to it, meaning that all
drivers provided by U8G2 can be used via this generic interface. This
integration is particularly useful to use monochrome drivers with higher-level
graphics libraries, such as the @ref pkg_lvgl package. For details on how to use
the generic API, check @ref drivers_disp_dev. The integration is provided by the
module @ref u8g2_disp_dev. It will provide default configurations, as well as
auto-initialization.

By default, the parameters defined in @ref u8g2_disp_dev_params are used to
initialize displays. The following examples show you how to configure a display
for I2C or SPI via the application's makefile. You can also override these
parameters in a header file of your board or application.

### I2C Display {#disp-dev-usage-i2c}

Let's say you have an I2C monochrome display with the 1306 controller, and you
want to use it with the generic display interface.

#### 1. Include the Modules

Add the following to your application's Makefile:

```Makefile
USEPKG += u8g2
USEMODULE += disp_dev
```
U8G2 supports both I2C and SPI to communicate with displays, so it will not
automatically pull any peripheral dependency. Therefore, you need to
additionally add the corresponding feature to the application's Makefile:

```Makefile
FEATURES_REQUIRED += periph_i2c
```

#### 2. Override Default Configurations

You'll need to configure the I2C device (bus), to which the display is connected
on your board, the I2C address, and the initialization function. To determine
the initialization function, refer to the setup documentation of the package:
https://github.com/olikraus/u8g2/wiki/u8g2setupc#setup-function-reference. You
can set these values in your Makefile, for example:

```Makefile
# I2C device 1, this will depend on which bus you connect the display on your
# board
CFLAGS += -DU8G2_DISPLAY_PARAM_DEV=I2C_DEV\(1\)

# I2C address, likely from your vendor's datasheet
CFLAGS += -DU8G2_DISPLAY_PARAM_I2C_ADDR=0x3C

# Initialization function taken from documentation
# https://github.com/olikraus/u8g2/wiki/u8g2setupc#ssd1306-128x64_noname-1
CFLAGS += -DU8G2_DISPLAY_PARAM_INIT_FUNCTION=u8g2_Setup_ssd1306_i2c_128x64_noname_f
```

### SPI Display #{disp-dev-usage-spi}

Let's say you have an SPI monochrome display with the 1306 controller, and you
want to use it with the generic display interface.

#### 1. Include the Modules

Add the following to your application's Makefile:

```Makefile
USEPKG += u8g2
USEMODULE += disp_dev
```

U8G2 supports both I2C and SPI to communicate with displays, so it will not
automatically pull any peripheral dependency. Therefore, you need to
additionally add the corresponding feature to the application's Makefile:

```Makefile
FEATURES_REQUIRED += periph_spi
```

#### 2. Override Default Configurations

You'll need to configure the SPI device (bus), to which the display is connected
on your board, set the I2C address to 0 (indicates that this is an SPI
connection), and the initialization function. To determine the initialization
function, refer to the setup documentation of the package:
https://github.com/olikraus/u8g2/wiki/u8g2setupc#setup-function-reference. You
can set these values in your Makefile, for example:

```Makefile
# SPI device 1, this will depend on which bus you connect the display on your
# board
CFLAGS += -DU8G2_DISPLAY_PARAM_DEV=SPI_DEV\(1\)

# We're using address 0 here to indicate that it's an SPI connection
CFLAGS += -DU8G2_DISPLAY_PARAM_I2C_ADDR=0

# Initialization function taken from documentation
# https://github.com/olikraus/u8g2/wiki/u8g2setupc#ssd1306-128x64_noname
CFLAGS += -DU8G2_DISPLAY_PARAM_INIT_FUNCTION=u8g2_Setup_ssd1306_128x64_noname_f
```

## Usage via Package API {#direct-usage}
If you prefer to use the package directly, you first need to add it to your
application's Makefile:
```Makefile
USEPKG += u8g2
```
Then, add the header in your code: `#include "u8g2.h"`.
Refer to the [U8g2 wiki](https://github.com/olikraus/u8g2/wiki) for more
information on the API.

### RIOT OS Port

The following two callbacks add support for the included drivers via I2C and
SPI peripherals:

* `u8x8_byte_hw_spi_riotos`
* `u8x8_byte_hw_i2c_riotos`

For timing and GPIO related operations, the following callback is available.

* `u8x8_gpio_and_delay_riotos`

These methods require a structure containing peripheral information
(`u8x8_riotos_t`), that is set using the `u8g2_SetUserPtr` function. This
structure contains the peripheral and pin mapping.

If the above interface is not sufficient, it is still possible to write a
dedicated interface by (re-)implementing the methods above. Refer to the
[U8g2 wiki](https://github.com/olikraus/u8g2/wiki) for more information.

### Example Using the Package Directly
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

u8g2_Setup_ssd1306_128x64_noname_1(&u8g2, U8G2_R0, u8x8_byte_riotos_hw_spi,
                                   u8x8_gpio_and_delay_riotos);
```

## Virtual Displays
For targets without an I2C or SPI, virtual displays are available. These
displays are part of U8g2, but are not compiled by default.

### UTF-8

By adding `USEMODULE += u8g2_utf8`, a terminal display is used as virtual
display, using UTF8 block characters that are printed to stdout.

Here's an example on how you would use the UTF-8 display:
```c
u8g2_t u8g2;

u8g2_SetupBuffer_Utf8(&u8g2, U8G2_R0);
```

### SDL Virtual Display
By adding `USEMODULE += u8g2_sdl`, a SDL virtual display will be used. This is
only available on native targets that have SDL installed. It uses `sdl2-config`
to find the headers and libraries. Note that RIOT-OS builds 32-bit binaries and
requires 32-bit SDL libraries. Using SDL requires more stack so in case you are
using it add the following to your Makefile:
```Makefile
CFLAGS += '-DTHREAD_STACKSIZE_MAIN= 48*1024'
```
48kB is enough for the test application in RIOT, other uses may need more or may
need this to be applied to other threads using `THREAD_STACKSIZE_DEFAULT`.
